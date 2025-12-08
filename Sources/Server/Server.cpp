#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>

#include "../Shared/InputState.hpp"
#include "../Shared/Player.hpp"
#include "../Shared/Projectile.hpp"
#include "../Shared/Utils.hpp"
#include "../Shared/Constants.hpp"

struct ClientInfo {
    int id;

    sf::TcpSocket *tcp;
    sf::IpAddress  udpIp;
    unsigned short udpPort = 0;

    Player *player;

    ClientInfo(int _id, sf::TcpSocket *newTcp, Player *newPlayer) 
        : id(_id), tcp(newTcp), player(newPlayer) {}
};

const int   PORT            = 55001;
const float SEND_HZ         = 30.0f;
const float SEND_INTEVAL    = 1.0f / SEND_HZ;
const float INTEREST_RADIUS = 800.0f;           // Only send entities within this distance

int main() {
    sf::TcpListener listener;
    if (listener.listen(PORT) != sf::Socket::Done) {
        std::cerr << "Cannot listen on port " << PORT << '\n';
        return -1;
    }
    listener.setBlocking(false);
    std::cout << "Server running TCP on port " << PORT << "..." << '\n';

    sf::UdpSocket udp;
    if (udp.bind(PORT) != sf::Socket::Done) {
        std::cerr << "Cannot bind UDP" << '\n';
        return -1;
    }
    udp.setBlocking(false);
    std::cout << "Server running UDP on port " << PORT << "..." << '\n';

    sf::SocketSelector selector;
    selector.add(listener);

    int nextClientId = 0;
    std::vector<ClientInfo> clients;
    std::unordered_map<sf::TcpSocket*, int> tcpToIndex;

    int nextProjectileId = 0;
    std::vector<Projectile*> projectiles;
    
    sf::Clock clock;
    float accumulator = 0.0f;
    sf::Clock sendClock;

    std::unordered_map<int, std::vector<InputState>> inputQueues;

    while (true) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        // === Non-blocking: Accept new TCP clients / read TCP inputs ===
        if (selector.wait(sf::milliseconds(1))) {
            // === Accept new client ===
            if (selector.isReady(listener)) {
                sf::TcpSocket *newTcp = new sf::TcpSocket();
                if (listener.accept(*newTcp) == sf::Socket::Done) {
                    newTcp->setBlocking(false);

                    int newId = ++nextClientId;

                    clients.emplace_back(
                        newId, 
                        newTcp, 
                        new Player(newId, sf::Vector2f(100.0f, 100.0f))
                    );
                    tcpToIndex[newTcp] = (int)clients.size() - 1;
                    inputQueues[newId] = {};

                    selector.add(*newTcp);

                    std::cout << "New client TCP connected. ID = " << newId << '\n';
                    
                    sf::Packet packet; packet << std::string("Assign_ID") << newId;
                    newTcp->send(packet);
                }
                else {
                    delete newTcp;
                }
            }

            // === Handle client packets ===
            for (size_t i = 0; i < clients.size(); ++i) {
                sf::TcpSocket &socket = *clients[i].tcp;
                if (selector.isReady(socket)) {
                    sf::Packet packet;
                    sf::Socket::Status status = socket.receive(packet);
                    if (status == sf::Socket::Done) {
                        std::string type; packet >> type;
                        if (type == "Input") {
                            InputState input;

                            packet >> input.seq >> input.movementDir.x >> input.movementDir.y >> input.isShooting;
                            
                            inputQueues[clients[i].id].push_back(input);
                        }
                    }
                    else if (status == sf::Socket::Disconnected) {
                        std::cout << "Client TCP disconnected ID = " << clients[i].id << "\n";

                        selector.remove(socket);
                        socket.disconnect();

                        delete clients[i].tcp;
                        delete clients[i].player;
                        
                        clients.erase(clients.begin() + i);
                        --i;

                        tcpToIndex.clear();
                        for (size_t k = 0; k < clients.size(); ++k) {
                            tcpToIndex[clients[k].tcp] = (int)k;
                        }
                    }
                }
            }
        }

        // === Receive UDP ===
        while (true) {
            sf::Packet packet;
            sf::IpAddress sender;
            unsigned short senderPort;
            sf::Socket::Status status = udp.receive(packet, sender, senderPort);
            if (status != sf::Socket::Done) break;
            std::string type; packet >> type;
            if (type == "Assign_UDP") {
                int clientId; packet >> clientId;
                for (ClientInfo &client : clients) {
                    if (client.id == clientId) {
                        client.udpIp = sender; client.udpPort = senderPort;
                        std::cout << "Register UDP endpoint for ID = " << clientId << " " << sender << ':' << senderPort << '\n';
                        break;
                    }
                }
            }
        }

        // === Fixed-timestep update loop for game logic ===
        while (accumulator >= SERVER_TICK) {
            for (ClientInfo &client : clients) {
                Player &player = *client.player;
                std::vector<InputState> &inputQueue = inputQueues[client.id];

                if (!inputQueue.empty()) {
                    InputState latest = inputQueue.back();
                    inputQueue.clear();

                    Projectile *bullet = player.updatePlayer(SERVER_TICK, latest);
                    if (bullet) {
                        bullet->setId(++nextProjectileId);
                        projectiles.push_back(bullet);
                    }

                    player.lastProcessedInput = latest.seq;
                }
                else {
                    InputState input;
                    player.updatePlayer(SERVER_TICK, input);
                }
            }

            for (size_t i = 0; i < projectiles.size(); ++i) {
                projectiles[i]->update(SERVER_TICK);
                
                if (projectiles[i]->isDestroyed() == false) {
                    for (ClientInfo &client : clients) {
                        Player &player = *client.player;
                        if (projectiles[i]->getOwnerId() != player.getId() && projectiles[i]->getBounds().intersects(player.getBounds())) {
                            player.takeDamage(projectiles[i]->getDamage());
                            projectiles[i]->destroy();
                            break;
                        }
                    }
                } 
                
                if (projectiles[i]->isDestroyed()) {
                    delete projectiles[i];
                    projectiles.erase(projectiles.begin() + i);
                    --i;
                }
            }

            accumulator -= SERVER_TICK;
        }

        // === Send packet for all clients ===
        if (sendClock.getElapsedTime().asSeconds() >= SEND_INTEVAL) {
            for (ClientInfo &client : clients) {
                if (client.udpPort == 0) continue;

                sf::Packet worldStatePacket;
                worldStatePacket << "WorldState";
    
                std::vector<Player*> visiblePlayers;
                for (size_t i = 0; i < clients.size(); ++i) {
                    Player *otherPlayer = clients[i].player;
                    if (distance(client.player->getPosition(), otherPlayer->getPosition()) <= INTEREST_RADIUS) {
                        visiblePlayers.push_back(otherPlayer);
                    }
                }
                
                worldStatePacket << (int)visiblePlayers.size();
                for (Player *player : visiblePlayers) {
                    worldStatePacket << player->getId() 
                                     << player->getPosition().x 
                                     << player->getPosition().y 
                                     << player->getHealth() 
                                     << player->lastProcessedInput;
                }
    
                // === Send packet for all projectiles ===
                std::vector<Projectile*> visibleProjectiles;
                for (Projectile *projectile : projectiles) {
                    if (distance(client.player->getPosition(), projectile->getPosition()) <= INTEREST_RADIUS || projectile->getOwnerId() == client.id) {
                        visibleProjectiles.push_back(projectile);
                    }
                }

                worldStatePacket << (int)visibleProjectiles.size();
                for (Projectile *projectile : visibleProjectiles) {
                    worldStatePacket << projectile->getId() 
                                     << projectile->getPosition().x 
                                     << projectile->getPosition().y 
                                     << projectile->getVelocity().x
                                     << projectile->getVelocity().y
                                     << projectile->getOwnerId();
                }

                udp.send(worldStatePacket, client.udpIp, client.udpPort);
            }
            
            sendClock.restart();
        }

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}