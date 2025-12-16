#include <SFML/Window/Event.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Network/Packet.hpp>

#include "../Shared/Constants.hpp"
#include "../Shared/Utils.hpp"

#include "NetworkClient.hpp"
#include "EntityManager.hpp"
#include "InputManager.hpp"
#include "Inventory.hpp"
#include "Renderer.hpp"
#include "Equipment.hpp"
#include "WorldCollision.hpp"
#include "DebugRenderer.hpp"

#include <iostream>

constexpr float CLIENT_HZ   = 60.0f;
constexpr float CLIENT_TICK = 1 / CLIENT_HZ;

void loginScreen(sf::RenderWindow &window, int &clientId, sf::Font &font) {
    bool loginDone = false;
    std::string inputClientId;

    sf::Clock caretClock;
    bool caretVisible = true;
    while (window.isOpen() && not loginDone) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) { // Backspace
                    if (!inputClientId.empty()) {
                        inputClientId.pop_back();
                    }
                }
                else if (event.text.unicode == 13) { // Enter
                    if (!inputClientId.empty()) {
                        loginDone = true;
                    }
                }
                else if (isdigit(event.text.unicode) && inputClientId.size() < 16) {
                    inputClientId += static_cast<char>(event.text.unicode);
                }
            }
        }

        if (caretClock.getElapsedTime().asSeconds() > 0.3f) {
            caretVisible = !caretVisible;
            caretClock.restart();
        }

        window.clear(sf::Color(30, 30, 30));

        sf::Text inputText;
        inputText.setFont(font);
        inputText.setCharacterSize(24);
        inputText.setPosition(100, 200);
        inputText.setString("Client ID: " + inputClientId + (caretVisible ? '|' : ' '));
        window.draw(inputText);

        window.display();
    }

    clientId = std::stoi(inputClientId);
}

void connectingScreen(sf::RenderWindow &window, NetworkClient &networkClient, int &clientId, sf::Font &font) {
    sf::Clock dotClock;
    int dotCount = 0;
    while (window.isOpen() && !networkClient.connectAll(clientId)) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }
        }

        if (dotClock.getElapsedTime().asSeconds() > 0.1f) {
            dotCount = (dotCount + 1) % 4; // 0 -> 3
            dotClock.restart();
        }

        window.clear(sf::Color(30, 30, 30));

        sf::Text connectingText;
        connectingText.setFont(font);
        connectingText.setCharacterSize(24);
        connectingText.setFillColor(sf::Color::White);
        connectingText.setPosition(100, 200);
        std::string dots(dotCount, '.');
        connectingText.setString("Connecting to server " + dots);
        window.draw(connectingText);

        window.display();
    }
}

int main() {
    bool isFullscreen = false;
    sf::RenderWindow window({ 800, 600 }, "[Client] - Login", sf::Style::Close);
    
    sf::Font font;
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }
    
    int clientId;
    loginScreen(window, clientId, font);

    NetworkClient networkClient("127.0.0.1", 55001, 55002);
    connectingScreen(window, networkClient, clientId, font);

    while (networkClient.assignedId == -1) {
        networkClient.pollReceive();
    }
    
    clientId = networkClient.assignedId;
    window.setTitle("[Client] - ID: " + std::to_string(clientId));

    EntityManager entityManager;
    Inventory inventory;
    Equipment equipment;

    Renderer renderer(window);

    WorldCollision worldCollision;
    DebugRenderer debugRenderer(window);

    int inputSeq = 0;
    std::vector<InputState> pendingInputs;

    sf::Clock clock;
    float clientAccumulator = 0.0f;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        clientAccumulator += dt;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                networkClient.close();
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    networkClient.close();
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::E) {
                    renderer.getInventoryUI().toggle();
                }
                else if (event.key.code == sf::Keyboard::F11) {
                    isFullscreen = !isFullscreen;

                    if (isFullscreen) {
                        window.create(
                            sf::VideoMode::getDesktopMode(),
                            "[Client] - ID: " + std::to_string(clientId),
                            sf::Style::Fullscreen
                        );
                        renderer.setCamera();
                    }
                    else {
                        window.create(
                            sf::VideoMode(800, 600),
                            "[Client] - ID: " + std::to_string(clientId),
                            sf::Style::Close
                        );
                        renderer.setCamera();
                    }
                }
                else if (event.key.code == sf::Keyboard::F3) {
                    debugRenderer.toggle();
                }
            }
            else if (renderer.getInventoryUI().isOpen()) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                        sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
                        renderer.getInventoryUI().handleLeftClick(worldPosition, inventory, equipment);
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
                    auto [from, to] = renderer.getInventoryUI().handleRelease(worldPosition);

                    if (from.first != -1 && from.second != -1 && to.first != -1 && to.second != -1) {       
                        if (from.first == to.first) {
                            if (from.first == 0) {
                                sf::Packet moveItemPacket;
                                moveItemPacket << "MoveItem" << from.second << to.second;
                                networkClient.sendTcpPacket(moveItemPacket);
                            }
                            else { // 1
                                // Equipment -> Equipment: Forbidden
                            }
                        }
                        else {
                            sf::Packet equipItemPacket; equipItemPacket << "EquipItem";
                            if (from.first == 0) {
                                equipItemPacket << from.second << to.second;
                                networkClient.sendTcpPacket(equipItemPacket);
                            }
                            else {
                                equipItemPacket << to.second << from.second;
                                networkClient.sendTcpPacket(equipItemPacket);
                            }
                        }
                    }
                }
            }
        }

        if (window.hasFocus() && not renderer.getInventoryUI().isOpen()) {
            InputState input;
            if (InputManager::getPlayerInput(input)) {
                input.seq = ++inputSeq;
                pendingInputs.push_back(input);

                networkClient.sendInputPacket(input.seq, input.movementDir, input.isShooting);
            
                if (entityManager.getPlayers().count(clientId) > 0) {
                    RemotePlayer &localPlayer = entityManager.getPlayer(clientId);
                    localPlayer.localPosition += normalize(input.movementDir) * PLAYER_SPEED * dt;
                }
            }
        }

        networkClient.pollReceive();
        
        WorldSnapshot &worldSnapshot = networkClient.getWorldSnapshot();
        if (worldSnapshot.appear) {
            worldSnapshot.appear = false;
            entityManager.applySnapshot(worldSnapshot, clientId, pendingInputs);
        }

        InventorySnapshot &inventorySnapshot = networkClient.getInventorySnapshot();
        if (inventorySnapshot.appear) {
            inventorySnapshot.appear = false;
            if ((int)inventory.getSlots().size() != (int)inventorySnapshot.itemIds.size()) {
                std::cout << "[Client] - InventorySnapshot receive bug!\n";
                return -1;
            }
            for (int index = 0; index < (int)inventorySnapshot.itemIds.size(); ++index) {
                inventory[index].itemId = inventorySnapshot.itemIds[index];
            }
        }

        EquipmentSnapshot &equipmentSnapshot = networkClient.getEquipmentSnapshot();
        if (equipmentSnapshot.appear) {
            equipmentSnapshot.appear = false;
            if ((int)equipment.getSlots().size() != (int)equipmentSnapshot.itemIds.size()) {
                std::cout << "[Client] - EquipmentSnapshot receive bug!\n";
                return -1;
            }
            for (int index = 0; index < (int)equipmentSnapshot.itemIds.size(); ++index) {
                equipment[index].itemId = equipmentSnapshot.itemIds[index];
            }
        }

        WorldCollisionSnapshot &worldCollisionSnapshot = networkClient.getWorldCollisionSnapshot();
        if (worldCollisionSnapshot.appear) {
            worldCollisionSnapshot.appear = false;
            for (const AABB &box : worldCollisionSnapshot.colliders) {
                worldCollision.getColliders().push_back(box);
            }
        }

        if (clientAccumulator >= CLIENT_TICK) {
            clientAccumulator -= CLIENT_TICK;
            entityManager.update(dt, clientId);
            
            window.clear(sf::Color(30.0f, 30.0f, 30.0f));
            
            renderer.render(entityManager, clientId);

            if (debugRenderer.isEnabled()) {
                debugRenderer.render(entityManager, worldCollision, clientId);
            }

            renderer.renderUI(inventory, equipment);

            window.display();
        }
        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}