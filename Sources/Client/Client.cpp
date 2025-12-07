#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "../Shared/InputManager.hpp"
#include "../Shared/Player.hpp"
#include "../Shared/Utils.hpp"

const std::string HOST = "127.0.0.1";
const int         PORT = 55001;

struct RemotePlayer {
    int id;
    sf::Vector2f serverPosition;
    sf::Vector2f localPosition;
    int hp;
    int lastAck = 0; // last processed input seq from server
};

struct RemoteProjectile {
    int id;
    sf::Vector2f serverPosition;
    sf::Vector2f velocity;
    sf::Vector2f localPosition;
    int ownerId;
    bool authoritative = false;
};

int main() {
    sf::TcpSocket tcp;
    if (tcp.connect(HOST , PORT, sf::seconds(1.f)) != sf::Socket::Done) {
        std::cout << "Cannot connect TCP to server " << HOST << ':' << PORT << '\n';
        return -1;
    }
    tcp.setBlocking(false);
    std::cout << "Connected TCP to server " << HOST << ':' << PORT << '\n';

    sf::UdpSocket udp;
    if (udp.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cout << "Cannot bind UDP to server " << HOST << ':' << PORT << '\n';
        return -1;
    }
    udp.setBlocking(false);
    std::cout << "Binding UDP to server " << HOST << ':' << PORT << '\n';

    sf::SocketSelector selector;
    selector.add(tcp);
    selector.add(udp);

    int myId = -1;
    int inputSeq = 0;
    std::vector<InputState> pendingInputs;

    std::unordered_map<int, RemotePlayer> remotePlayers;
    std::unordered_map<int, RemoteProjectile> remoteProjectiles;
    std::vector<RemoteProjectile> localPredictedProjectiles;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Client");

    sf::Clock clock;

    sf::Vector2f localPosition(100.0f, 100.0f);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                tcp.disconnect();
                udp.unbind();
                window.close();
            }
        }

        if (window.hasFocus()) {
            InputState input;
            if (InputManager::getPlayerInput(input)) {
                input.seq = ++inputSeq;

                localPosition += input.movementDir * PLAYER_SPEED * dt;
                // localPosition += input.movementDir * PLAYER_SPEED * SERVER_TICK;

                pendingInputs.push_back(input);

                sf::Packet inputPacket;
                inputPacket << std::string("Input");
                inputPacket << input.seq << input.movementDir.x << input.movementDir.y << input.isShooting;
                tcp.send(inputPacket);
            }
        }

        if (selector.wait(sf::milliseconds(1))) {
            if (selector.isReady(tcp)) {
                sf::Packet packet;
                sf::Socket::Status status = tcp.receive(packet);
                if (status == sf::Socket::Done) {
                    std::string type; packet >> type;
                    if (type == "Assign_ID") {
                        packet >> myId;
                        std::cout << "My ID assigned by server = " << myId << '\n';

                        sf::Packet assignUdp;
                        assignUdp << std::string("Assign_UDP") << myId;
                        udp.send(assignUdp, HOST, PORT);
                    }
                }
                else if (status == sf::Socket::Disconnected) {
                    std::cout << "Disconnected from server" << '\n';
                    window.close();
                }
            }

            if (selector.isReady(udp)) {
                sf::Packet packet; 
                sf::IpAddress sender;
                unsigned short port;
                sf::Socket::Status status = udp.receive(packet, sender, port);
                if (status == sf::Socket::Done) {
                    std::string type; packet >> type;
                    if (type == "WorldState") {
                        int playerCount; packet >> playerCount;
                        std::unordered_map<int, RemotePlayer> newPlayers;
                        for (int i = 0; i < playerCount; ++i) {
                            int id, hp, lastProcessed;
                            float x, y;
                            packet >> id >> x >> y >> hp >> lastProcessed;
                            RemotePlayer remotePlayer;
                            remotePlayer.id = id;
                            remotePlayer.serverPosition = {x, y};
                            remotePlayer.hp = hp;
                            remotePlayer.lastAck = lastProcessed;

                            if (remotePlayers.count(id)) {
                                remotePlayer.localPosition = remotePlayers[id].localPosition;
                            }
                            else {
                                remotePlayer.localPosition = remotePlayer.serverPosition;
                            }

                            if (id == myId) {
                                while (!pendingInputs.empty() && pendingInputs.front().seq <= lastProcessed) {
                                    pendingInputs.erase(pendingInputs.begin());
                                }

                                sf::Vector2f reconciled = remotePlayer.serverPosition;
                                for (InputState &input : pendingInputs) {
                                    // reconciled += input.movementDir * PLAYER_SPEED * dt;
                                    reconciled += input.movementDir * PLAYER_SPEED * SERVER_TICK;
                                }

                                localPosition = lerp(localPosition, reconciled, 0.6f);
                            }

                            newPlayers[id] = remotePlayer;
                        }
                        remotePlayers.swap(newPlayers);

                        int projectileCount; packet >> projectileCount;
                        std::unordered_map<int, RemoteProjectile> newProjectiles;
                        for (int i = 0; i < projectileCount; ++i) {
                            int projectileId; float x, y, vx, vy; int ownerId = -1;
                            packet >> projectileId >> x >> y >> vx >> vy >> ownerId;
                            RemoteProjectile projectile;
                            projectile.id             = projectileId;
                            projectile.serverPosition = { x, y };
                            projectile.velocity       = { vx, vy };
                            projectile.localPosition  = projectile.serverPosition;
                            projectile.ownerId        = ownerId;
                            projectile.authoritative  = true;

                            // Nếu đã tồn tại, giữ localPosition để bay mượt
                            if (remoteProjectiles.count(projectileId)) {
                                projectile.localPosition = remoteProjectiles[projectileId].localPosition;
                            } 
                            else {
                                projectile.localPosition = projectile.serverPosition;
                            }

                            newProjectiles[projectileId] = projectile;
                        }

                        for (auto it = localPredictedProjectiles.begin(); it != localPredictedProjectiles.end(); ) {
                            bool matched = false;
                            for (auto &[id, projectile] : newProjectiles) {
                                if (projectile.ownerId == myId && dist2(projectile.serverPosition, it->localPosition) < 40.0f) {
                                    projectile.localPosition = it->localPosition;
                                    matched = true;
                                    break;
                                }
                            }
                            if (matched) it = localPredictedProjectiles.erase(it);
                            else ++it;
                        }
                 
                        for (auto &[id, newProj] : newProjectiles) {
                            if (remoteProjectiles.count(id)) {
                                remoteProjectiles[id].serverPosition = newProj.serverPosition;
                                remoteProjectiles[id].velocity = newProj.velocity;
                            } 
                            else {
                                remoteProjectiles[id] = newProj;
                            }
                        }

                        for (auto it = remoteProjectiles.begin(); it != remoteProjectiles.end(); ) {
                            if (newProjectiles.count(it->first) == 0) {
                                it = remoteProjectiles.erase(it);
                            } else {
                                ++it;
                            }
                        }
                        // remoteProjectiles.swap(newProjectiles);
                    }
                }
            }
        }

        for (auto &[id, remotePlayer] : remotePlayers) {
            if (remotePlayer.id != myId) {
                remotePlayer.localPosition = lerp(remotePlayer.localPosition, remotePlayer.serverPosition, 0.2f);
            }
            else {
                remotePlayer.localPosition = localPosition;
            }
        }

        for (RemoteProjectile &projectile : localPredictedProjectiles) {
            projectile.localPosition += projectile.velocity * dt;
            // projectile.localPosition += projectile.velocity * SERVER_TICK;
        }

        for (auto &[id, projectile] : remoteProjectiles) {
            projectile.localPosition += projectile.velocity * dt;
            // projectile.localPosition += projectile.velocity * SERVER_TICK;
            float distanceToServer = distance(projectile.localPosition, projectile.serverPosition);
            if (distanceToServer > 50.0f) {
                projectile.localPosition = lerp(projectile.localPosition, projectile.serverPosition, 0.3f);
            }
        }

        window.clear(sf::Color(30, 30, 30));

        sf::RectangleShape playerShape(sf::Vector2f(40.0f, 40.0f));
        playerShape.setOrigin(playerShape.getSize() / 2.0f);
        for (auto &[id, remotePlayer] : remotePlayers) {
            playerShape.setPosition(remotePlayer.localPosition);
            playerShape.setFillColor(remotePlayer.id == myId ? sf::Color::Blue : sf::Color::Red);
            
            window.draw(playerShape);
        }
        if (remotePlayers.find(myId) == remotePlayers.end()) {
            playerShape.setPosition(localPosition);
            playerShape.setFillColor(sf::Color::Blue);
            
            window.draw(playerShape);
        }

        sf::CircleShape projectileShape(5.0f);
        projectileShape.setOrigin(5.0f, 5.0f);
        for (auto &[id, projectile] : remoteProjectiles) {
            projectileShape.setPosition(projectile.localPosition);
            projectileShape.setFillColor(projectile.ownerId == myId ? sf::Color::Yellow : sf::Color::White);
            
            window.draw(projectileShape);
        }

        for (RemoteProjectile &projectile : localPredictedProjectiles) {
            projectileShape.setPosition(projectile.localPosition);
            projectileShape.setFillColor(sf::Color::Yellow);

            window.draw(projectileShape);
        }

        window.display();

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}