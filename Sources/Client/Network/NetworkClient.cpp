#include "Client/Network/NetworkClient.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Network/Packet.hpp>

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
    loginPacket << "Login" << clientId;
    tcp.send(loginPacket);
}

void NetworkClient::pollTCP() {
    if (!selector.wait(sf::milliseconds(1))) {
        return;
    }

    if (!selector.isReady(tcp)) {
        return;
    }

    sf::Packet packet;
    sf::Socket::Status status = tcp.receive(packet);
    if (status != sf::Socket::Done) {
        if (status == sf::Socket::NotReady) {
            // std::cout << "[Network] - TCP not ready" << '\n';
        }
        else if (status == sf::Socket::Disconnected) {
            // std::cout << "[Network] - TCP disconnected" << '\n';
        }
        return;
    }

    std::string type; packet >> type;
    if (type == "Assign_ID") {
        packet >> assignedId;

        std::cout << "[Network] - Assigned ID: " << assignedId << '\n';

        sf::Packet assignUdp;
        assignUdp << std::string("Assign_UDP") << assignedId;
        udp.send(assignUdp, host, udpPort);

        loginStatus = LoginStatus::Success;
    }
    else if (type == "Login_Fail") {
        loginStatus = LoginStatus::Fail;
    }
    else if (type == "Inventory") {
        inventorySnapshot.appear = true;
        inventorySnapshot.itemIds.clear();
        
        int inventorySize; packet >> inventorySize;
        for (int i = 0; i < inventorySize; ++i) {
            int itemId; packet >> itemId;
            inventorySnapshot.itemIds.push_back(itemId);
        }
    }
    else if (type == "Equipment") {
        equipmentSnapshot.appear = true;
        equipmentSnapshot.itemIds.clear();
        
        int equipmentSize; packet >> equipmentSize;
        for (int i = 0; i < equipmentSize; ++i) {
            int itemId; packet >> itemId;
            equipmentSnapshot.itemIds.push_back(itemId);
        }
    }
    else if (type == "WorldCollision") {
        worldCollisionSnapshot.appear = true;
        worldCollisionSnapshot.colliders.clear();

        int collidersSize; packet >> collidersSize;
        for (int i = 0; i < collidersSize; ++i) {
            sf::Vector2f position, size;
            packet >> position.x >> position.y >> size.x >> size.y;
            worldCollisionSnapshot.colliders.push_back({ position, size });
        }
    }
    else if (type == "Chunk") {
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
    else {
        std::cout << "[Network] - Unknown pollReceive() TCP msg type: " << type << '\n';
    }
}

void NetworkClient::pollUDP() {
    if (!selector.wait(sf::milliseconds(1))) {
        return;
    }

    if (!selector.isReady(udp)) {
        return;
    }

    sf::Packet packet; sf::IpAddress sender; unsigned short senderPort;
    sf::Socket::Status status = udp.receive(packet, sender, senderPort);
    if (status != sf::Socket::Done) {
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
    }
    else {
        std::cout << "[Network] - Unknown UDP packet type: " << type << '\n';
    }
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

void NetworkClient::close() {
    tcp.disconnect();
    udp.unbind();
}