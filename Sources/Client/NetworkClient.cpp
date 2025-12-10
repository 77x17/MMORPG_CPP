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

std::optional<WorldSnapshot> NetworkClient::pollReceive() {
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
                    WorldSnapshot worldSnapshot;
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

                    int projectileCount; packet >> projectileCount;
                    for (int i = 0; i < projectileCount; ++i) {
                        ProjectileSnapshot projectileSnapshot; 
                        packet >> projectileSnapshot.id 
                               >> projectileSnapshot.x 
                               >> projectileSnapshot.y 
                               >> projectileSnapshot.vx 
                               >> projectileSnapshot.vy 
                               >> projectileSnapshot.ownerId;

                        worldSnapshot.projectiles.push_back(projectileSnapshot);
                    }

                    return worldSnapshot;
                }
                else {
                    std::cout << "[Network] - UDP packet type: " << type << '\n';
                }
            }
        }
    }

    return std::nullopt;
}

void NetworkClient::close() {
    tcp.disconnect();
    udp.unbind();
}