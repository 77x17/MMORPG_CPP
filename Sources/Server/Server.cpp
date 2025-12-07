#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "../Shared/InputState.hpp"
#include "../Shared/Player.hpp"
#include "../Shared/Projectile.hpp"
#include "../Shared/Utils.hpp"

struct ClientInfo {
    int id;
    sf::TcpSocket *socket;

    Player *player;

    ClientInfo(int _id, sf::TcpSocket *newSocket, Player *newPlayer) : id(_id), socket(newSocket), player(newPlayer) {}
};

const int   PORT         = 55001;
const float SEND_HZ      = 30.f;
const float SEND_INTEVAL = 1.f / SEND_HZ;

int main() {
    sf::TcpListener listener;
    if (listener.listen(PORT) != sf::Socket::Done) {
        std::cerr << "Cannot listen on port " << PORT << '\n';
    }
    listener.setBlocking(false);
    std::cout << "Server started on port " << PORT << "..." << '\n';

    sf::SocketSelector selector;
    selector.add(listener);

    int nextClientId = 0;
    std::vector<ClientInfo> clients;
    std::unordered_map<sf::TcpSocket*, int> socketToIndex;

    std::vector<Projectile*> projectiles;
    
    sf::Clock clock;
    float accumulator = 0.f;
    sf::Clock sendClock;

    std::unordered_map<int, std::vector<InputState>> inputQueues;

    while (true) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        if (selector.wait(sf::milliseconds(1))) {
            // === Accept new client ===
            if (selector.isReady(listener)) {
                sf::TcpSocket *newSocket = new sf::TcpSocket();
                if (listener.accept(*newSocket) == sf::Socket::Done) {
                    newSocket->setBlocking(false);

                    int newId = ++nextClientId;

                    clients.emplace_back(
                        newId, 
                        newSocket, 
                        new Player(newId, sf::Vector2f(100, 100), sf::Color::White)
                    );
                    socketToIndex[newSocket] = (int)clients.size() - 1;
                    inputQueues[newId] = {};

                    selector.add(*newSocket);

                    std::cout << "New client connected. ID = " << newId << '\n';
                    
                    sf::Packet packet;
                    packet << std::string("Assign_ID") << newId;
                    newSocket->send(packet);
                }
                else {
                    delete newSocket;
                }
            }

            // === Handle client packets ===
            for (size_t i = 0; i < clients.size(); ++i) {
                sf::TcpSocket &socket = *clients[i].socket;
                if (selector.isReady(socket)) {
                    sf::Packet packet;
                    sf::Socket::Status status = socket.receive(packet);
                    if (status == sf::Socket::Done) {
                        std::string type; packet >> type;
                        if (type == "Input") {
                            InputState input;

                            packet >> input.seq >> input.movementDir.x >> input.movementDir.y >> input.isShooting;
                            
                            inputQueues[clients[i].id].push_back(input);

                            // Player &player = *clients[i].player;
                            // Projectile *bullet = player.updatePlayer(dt, input);
                            // if (bullet) {
                            //     projectiles.push_back(bullet);
                            // }
                        }
                    }
                    else if (status == sf::Socket::Disconnected) {
                        int cid = clients[i].id;
                        std::cout << "Client disconnected ID = " << cid << "\n";

                        selector.remove(socket);
                        socket.disconnect();

                        delete clients[i].socket;
                        delete clients[i].player;
                        
                        clients.erase(clients.begin() + i);
                        --i;

                        socketToIndex.clear();
                        for (size_t k = 0; k < clients.size(); ++k) {
                            socketToIndex[clients[k].socket] = (int)k;
                        }
                    }
                }
            }
        }

        // === Fixed-timestep update loop for game logic ===
        while (accumulator >= SERVER_TICK) {
            for (ClientInfo &client : clients) {
                int id = client.id;
                Player &player = *client.player;
                std::vector<InputState> &inputQueue = inputQueues[id];

                if (!inputQueue.empty()) {
                    InputState latest = inputQueue.back();
                    inputQueue.clear();

                    Projectile *bullet = player.updatePlayer(SERVER_TICK, latest);
                    if (bullet) {
                        projectiles.push_back(bullet);
                    }

                    player.lastProcessedInput = latest.seq;
                }
            }

            for (size_t i = 0; i < projectiles.size(); ++i) {
                projectiles[i]->update(SERVER_TICK);
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
            sf::Packet worldStatePacket;
            worldStatePacket << "WorldState";

            worldStatePacket << (int)clients.size();
            for (size_t i = 0; i < clients.size(); ++i) {
                Player &player = *clients[i].player;
                worldStatePacket << player.getId() << player.getPosition().x << player.getPosition().y << player.getHealth() << player.lastProcessedInput;
            }

            // === Send packet for all projectiles ===
            worldStatePacket << (int)projectiles.size();
            for (size_t i = 0; i < projectiles.size(); ++i) {
                worldStatePacket << projectiles[i]->getPosition().x << projectiles[i]->getPosition().y << projectiles[i]->getOwnerId();
            }
            
            for (size_t i = 0; i < clients.size(); ++i) {
                sf::TcpSocket &socket = *clients[i].socket;
                socket.send(worldStatePacket);
            }

            sendClock.restart();
        }

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}