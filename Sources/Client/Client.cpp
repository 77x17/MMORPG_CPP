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
    sf::Vector2f position;
    int ownerId;
};

int main() {
    sf::TcpSocket socket;
    if (socket.connect(HOST , PORT, sf::seconds(1.f)) != sf::Socket::Done) {
        std::cout << "Cannot connect to server " << HOST << ':' << PORT << '\n';
        return 0;
    }
    socket.setBlocking(false);
    std::cout << "Connected to server " << HOST << ':' << PORT << '\n';

    sf::SocketSelector selector;
    selector.add(socket);

    int myId = -1;
    int inputSeq = 0;

    std::unordered_map<int, RemotePlayer> remotePlayers;
    std::vector<RemoteProjectile> remoteProjectiles;
    std::vector<InputState> pendingInputs;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Client");

    sf::Clock clock;

    sf::Vector2f localPosition(100.0f, 100.0f);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                socket.disconnect();
                window.close();
            }
        }

        if (window.hasFocus()) {
            InputState input;
            if (InputManager::getPlayerInput(input)) {
                input.seq = ++inputSeq;

                localPosition += input.movementDir * PLAYER_SPEED * dt;

                pendingInputs.push_back(input);

                sf::Packet inputPacket;
                inputPacket << std::string("Input");
                inputPacket << input.seq << input.movementDir.x << input.movementDir.y << input.isShooting;
                socket.send(inputPacket);
            }
        }

        if (selector.wait(sf::milliseconds(1))) {
            if (selector.isReady(socket)) {
                sf::Packet packet;
                sf::Socket::Status status = socket.receive(packet);
                if (status == sf::Socket::Done) {
                    std::string type; packet >> type;
                    if (type == "Assign_ID") {
                        packet >> myId;
                        std::cout << "My ID assigned by server = " << myId << '\n';
                    }
                    else if (type == "WorldState") {
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

                                sf::Vector2f position = remotePlayer.serverPosition;
                                for (InputState &input : pendingInputs) {
                                    position += input.movementDir * PLAYER_SPEED * SERVER_TICK;
                                }

                                localPosition = lerp(localPosition, position, 0.3f);
                            }

                            newPlayers[id] = remotePlayer;
                        }

                        int projectileCount; packet >> projectileCount;
                        std::vector<RemoteProjectile> newProjectiles;
                        for (int i = 0; i < projectileCount; ++i) {
                            float x, y;
                            int ownerId = -1;
                            packet >> x >> y >> ownerId;
                            newProjectiles.push_back({{x, y}, ownerId});
                        }

                        remotePlayers.swap(newPlayers);
                        remoteProjectiles.swap(newProjectiles);
                    }
                }
                else if (status == sf::Socket::Disconnected) {
                    std::cout << "Disconnected from server" << '\n';
                    window.close();
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

        window.clear(sf::Color(20, 20, 20));

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
        for (RemoteProjectile &projectile : remoteProjectiles) {
            projectileShape.setPosition(projectile.position);
            projectileShape.setFillColor(projectile.ownerId == myId ? sf::Color::Yellow : sf::Color::White);
            
            window.draw(projectileShape);
        }

        window.display();

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}