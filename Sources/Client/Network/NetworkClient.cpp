#include "Client/Network/NetworkClient.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Network/Packet.hpp>

#include "Shared/PacketType.hpp"

#include <iostream>

NetworkClient::NetworkClient(const std::string &_host, unsigned short _tcpPort, unsigned short _udpPort) 
: host(_host), tcpPort(_tcpPort), udpPort(_udpPort) {
    selector.clear();
}

NetworkClient::~NetworkClient() {
    close();
}

bool NetworkClient::connectTcp(const float &timeoutSeconds) {
    if (tcp.connect(host, tcpPort, sf::seconds(timeoutSeconds)) != sf::Socket::Done) {
        // std::cout << "[Network] - Cannot connect TCP" << '\n';
        return false;
    }
    tcp.setBlocking(false);
    selector.add(tcp);

    return true;
}

// already bound in connectTcp path
bool NetworkClient::bindUdp() {
    if (udp.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        // std::cout << "[Network] - Cannot bind UDP" << '\n';
        return false;
    }
    udp.setBlocking(false);
    selector.add(udp);

    return true;
}

bool NetworkClient::connectAll(const float& timeoutSeconds) {
    if (isConnected) {
        return true;
    }

    if (connectTcp(timeoutSeconds) && bindUdp()) {
        std::cout << "[Network] - Connected to server..." << '\n';
        return isConnected = true;    
    }

    return false;
}

void NetworkClient::update(float dt) {
    tcpPingTracker.update(dt);
    udpPingTracker.update(dt);

    if (tcpPingTracker.shouldSendPing()) {
        sf::Packet packet;
        packet << static_cast<uint8_t>(PacketType::TcpPing) << TcpPingTracker::nowMs();
        tcp.send(packet);
        tcpPingTracker.onPingSent();
    }
    if (udpPingTracker.shouldSendPing()) {
        sf::Packet packet;
        uint64_t time = udpPingTracker.onPingSent();
        packet << "UdpPing" << time;
        udp.send(packet, host, udpPort);
    }
}

void NetworkClient::sendInputPacket(int seq, const sf::Vector2f &moveDir, bool isShooting) {
    sf::Packet packet;
    packet << std::string("Input") << assignedId << seq << moveDir.x << moveDir.y << isShooting;
    udp.send(packet, host, udpPort);
}

void NetworkClient::sendTcpPacket(sf::Packet &packet) {
    tcp.send(packet);
}

void NetworkClient::sendLogin(int clientId) {
    sf::Packet loginPacket;
    loginPacket << static_cast<uint8_t>(PacketType::Login) << clientId;
    tcp.send(loginPacket);
}

void NetworkClient::pollTCP() {
    // for (int i = 0; i < 10; ++i) {
        sf::Packet packet;
        sf::Socket::Status status = tcp.receive(packet);
        if (status != sf::Socket::Done) {
            if (status == sf::Socket::NotReady) {
                // std::cout << "[Network] - TCP not ready" << '\n';
            }
            else if (status == sf::Socket::Disconnected) {
                // std::cout << "[Network] - TCP disconnected" << '\n';
            }
            // break;
            return;
        }

        uint8_t type; packet >> type;
        if (static_cast<PacketType>(type) == PacketType::Assign_ID) {
            packet >> assignedId;

            std::cout << "[Network] - Assigned ID: " << assignedId << '\n';

            sf::Packet assignUdp;
            assignUdp << std::string("Assign_UDP") << assignedId;
            udp.send(assignUdp, host, udpPort);

            loginStatus = LoginStatus::Success;
        }
        else if (static_cast<PacketType>(type) == PacketType::Login_Fail) {
            loginStatus = LoginStatus::Fail;
        }
        else if (static_cast<PacketType>(type) == PacketType::Inventory) {
            inventorySnapshot.appear = true;
            inventorySnapshot.itemIds.clear();
            
            int inventorySize; packet >> inventorySize;
            for (int i = 0; i < inventorySize; ++i) {
                int itemId; packet >> itemId;
                inventorySnapshot.itemIds.push_back(itemId);
            }
        }
        else if (static_cast<PacketType>(type) == PacketType::Equipment) {
            equipmentSnapshot.appear = true;
            equipmentSnapshot.itemIds.clear();
            
            int equipmentSize; packet >> equipmentSize;
            for (int i = 0; i < equipmentSize; ++i) {
                int itemId; packet >> itemId;
                equipmentSnapshot.itemIds.push_back(itemId);
            }
        }
        else if (static_cast<PacketType>(type) == PacketType::WorldCollision) {
            worldCollisionSnapshot.appear = true;
            worldCollisionSnapshot.colliders.clear();

            int collidersSize; packet >> collidersSize;
            for (int i = 0; i < collidersSize; ++i) {
                sf::Vector2f position, size;
                packet >> position.x >> position.y >> size.x >> size.y;
                worldCollisionSnapshot.colliders.push_back({ position, size });
            }
        }
        else if (static_cast<PacketType>(type) == PacketType::Chunk) {
            chunkSnapshot.appear = true;
            chunkSnapshot.chunks.clear();

            int chunkCount; packet >> chunkCount;
            for (int i = 0; i < chunkCount; ++i) {
                int x, y, size;
                packet >> x
                    >> y
                    >> size
                    >> size;
                chunkSnapshot.chunks.push_back({ sf::Vector2f(x, y), sf::Vector2f(size, size) });
            }
        } 
        else if (static_cast<PacketType>(type) == PacketType::TcpPing) {
            uint64_t timeMs; packet >> timeMs;
            tcpPingTracker.onPingResponse(timeMs);
        }
        else {
            std::cout << "[Network] - Unknown pollReceive() TCP msg type: " << type << '\n';
        }
    // }
}

void NetworkClient::pollUDP() {
    // for (int i = 0; i < 5; ++i) {
        sf::Packet packet; sf::IpAddress sender; unsigned short senderPort;
        sf::Socket::Status status = udp.receive(packet, sender, senderPort);
        if (status != sf::Socket::Done) {
            // break;
            return;
        }

        std::string type; packet >> type;
        if (type == "WorldState") {
            worldSnapshot.appear = true;
            worldSnapshot.players.clear();
            int playerCount; packet >> playerCount;
            for (int i = 0; i < playerCount; ++i) {
                PlayerSnapshot playerSnapshot; 
                packet >> playerSnapshot.id 
                        >> playerSnapshot.x 
                        >> playerSnapshot.y 
                        >> playerSnapshot.hp
                        >> playerSnapshot.lastProcessed;

                worldSnapshot.players.push_back(playerSnapshot); 
            }

            int damageEntityCount; packet >> damageEntityCount;
            worldSnapshot.projectiles.clear();
            worldSnapshot.swordSlashs.clear();
            for (int i = 0; i < damageEntityCount; ++i) {
                std::string type; packet >> type;
                if (type == "Projectile") {
                    ProjectileSnapshot projectileSnapshot; 
                    packet >> projectileSnapshot.id 
                            >> projectileSnapshot.x 
                            >> projectileSnapshot.y 
                            >> projectileSnapshot.vx 
                            >> projectileSnapshot.vy 
                            >> projectileSnapshot.ownerId;
                    worldSnapshot.projectiles.push_back(projectileSnapshot);
                }
                else if (type == "SwordSlash") {
                    SwordSlashSnapshot swordSlashSnapshot;
                    packet >> swordSlashSnapshot.id 
                            >> swordSlashSnapshot.left 
                            >> swordSlashSnapshot.top 
                            >> swordSlashSnapshot.width
                            >> swordSlashSnapshot.height
                            >> swordSlashSnapshot.ownerId;
                    worldSnapshot.swordSlashs.push_back(swordSlashSnapshot);
                }
                else {
                    std::cout << "[Network] - Unknown type in DamageEntity: " << type << '\n';
                }
            }

            int enemyCount; packet >> enemyCount;
            worldSnapshot.enemies.clear();
            for (int i = 0; i < enemyCount; ++i) {
                EnemySnapshot enemySnapshot;
                packet >> enemySnapshot.id
                       >> enemySnapshot.x
                       >> enemySnapshot.y
                       >> enemySnapshot.hp;
                worldSnapshot.enemies.push_back(enemySnapshot);
            }

            bool mouseSelected; packet >> mouseSelected;
            if (mouseSelected) {
                int id, hp, maxHp;
                packet >> id >> hp >> maxHp;
                worldSnapshot.mouseSelected.appear = true;
                worldSnapshot.mouseSelected.id = id;
                worldSnapshot.mouseSelected.hp = hp;
                worldSnapshot.mouseSelected.maxHp = maxHp;
                worldSnapshot.mouseSelected.name = "Enemy";
            }
        }
        else if (type == "UdpPing") {
            uint64_t timeMs; packet >> timeMs;
            udpPingTracker.onPingResponse(timeMs);
        }
        else {
            std::cout << "[Network] - Unknown UDP packet type: " << type << '\n';
        }
    // }
}

void NetworkClient::poll() {
    pollTCP();
    pollUDP();
}

int NetworkClient::getClientId() const {
    return assignedId;
}

void NetworkClient::resetLoginStatus() {
    loginStatus = LoginStatus::None;
}

LoginStatus NetworkClient::getLoginStatus() const {
    return loginStatus;
}

ChunkSnapshot & NetworkClient::getChunkSnapshot() {
    return chunkSnapshot;
}

EquipmentSnapshot & NetworkClient::getEquipmentSnapshot() {
    return equipmentSnapshot;
}

InventorySnapshot & NetworkClient::getInventorySnapshot() {
    return inventorySnapshot;
}

WorldCollisionSnapshot & NetworkClient::getWorldCollisionSnapshot() {
    return worldCollisionSnapshot;
}

WorldSnapshot & NetworkClient::getWorldSnapshot() {
    return worldSnapshot;
}

int NetworkClient::getTcpPing() const {
    return tcpPingTracker.getPing();
}

int NetworkClient::getUdpPing() const {
    return udpPingTracker.getPing();
}

void NetworkClient::close() {
    tcp.disconnect();
    udp.unbind();
}