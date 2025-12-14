#include "NetworkClient.hpp"

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
        std::cout << "[Network] - Cannot connect TCP" << '\n';
        return false;
    }
    tcp.setBlocking(false);
    selector.add(tcp);

    return true;
}

// already bound in connectTcp path
bool NetworkClient::bindUdp() {
    if (udp.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cout << "[Network] - Cannot bind UDP" << '\n';
        return false;
    }
    udp.setBlocking(false);
    selector.add(udp);

    return true;
}

bool NetworkClient::connectAll(const float& timeoutSeconds) {
    if (connectTcp(timeoutSeconds) && bindUdp()) {
        std::cout << "[Network] - Connected to server..." << '\n';
        return true;    
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

void NetworkClient::pollReceive() {
    if (selector.wait(sf::milliseconds(1))) {
        if (selector.isReady(tcp)) {
            sf::Packet packet; sf::Socket::Status status = tcp.receive(packet);
            if (status == sf::Socket::Done) {
                std::string type; packet >> type;
                if (type == "Assign_ID") {
                    packet >> assignedId;
                    std::cout << "[Network] - Assigned ID: " << assignedId << '\n';

                    sf::Packet assignUdp;
                    assignUdp << std::string("Assign_UDP") << assignedId;
                    udp.send(assignUdp, host, udpPort);
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
                else {
                    std::cout << "[Network] - TCP msg type: " << type << '\n';
                }
            }
            else if (status == sf::Socket::NotReady) {

            }
            else if (status == sf::Socket::Disconnected) {
                std::cout << "[Network] - TCP disconnected" << '\n';
            }
        }

        if (selector.isReady(udp)) {
            sf::Packet packet; sf::IpAddress sender; unsigned short senderPort;
            sf::Socket::Status status = udp.receive(packet, sender, senderPort);
            if (status == sf::Socket::Done) {
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

                    int damageEntitiesCount; packet >> damageEntitiesCount;
                    worldSnapshot.projectiles.clear();
                    worldSnapshot.swordSlashs.clear();
                    for (int i = 0; i < damageEntitiesCount; ++i) {
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
                }
                else {
                    std::cout << "[Network] - UDP packet type: " << type << '\n';
                }
            }
        }
    }
}

WorldSnapshot & NetworkClient::getWorldSnapshot() {
    return worldSnapshot;
}

InventorySnapshot & NetworkClient::getInventorySnapshot() {
    return inventorySnapshot;
}

void NetworkClient::close() {
    tcp.disconnect();
    udp.unbind();
}