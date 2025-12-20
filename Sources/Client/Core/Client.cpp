#include <SFML/Window/Event.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Network/Packet.hpp>

#include "Shared/Constants.hpp"
#include "Shared/Utils.hpp"

#include "Client/Core/Login.hpp"
#include "Client/Core/Renderer.hpp"
#include "Client/Debug/DebugRenderer.hpp"
#include "Client/Inventory/Inventory.hpp"
#include "Client/Inventory/Equipment.hpp"
#include "Client/Network/NetworkClient.hpp"
#include "Client/Systems/Entity/EntityManager.hpp"
#include "Client/Systems/Input/InputManager.hpp"
#include "Client/Systems/World/WorldCollision.hpp"

#include "Client/Utils/Constants.hpp"
#include "Client/Utils/Font.hpp"

#include <iostream>

sf::Font Font::font;

int main() {
    bool isFullscreen = false;
    sf::RenderWindow window({ WINDOW_WIDTH, WINDOW_HEIGHT }, "[Client] - Login", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    Font::loadFromFile("Assets/Roboto_Mono.ttf");
    
    NetworkClient networkClient("127.0.0.1", 55001, 55002);
    
    Login login;
    login.connecting(window, networkClient);
    
    window.setTitle("[Client] - ID: " + std::to_string(networkClient.getClientId()));

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
                            "[Client] - ID: " + std::to_string(networkClient.getClientId()),
                            sf::Style::Fullscreen
                        );
                        window.setVerticalSyncEnabled(true);

                        renderer.setCamera();
                    }
                    else {
                        window.create(
                            sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                            "[Client] - ID: " + std::to_string(networkClient.getClientId()),
                            sf::Style::Close
                        );
                        window.setVerticalSyncEnabled(true);

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
            
                if (entityManager.getPlayers().count(networkClient.getClientId()) > 0) {
                    RemotePlayer &localPlayer = entityManager.getPlayer(networkClient.getClientId());
                    localPlayer.localPosition += normalize(input.movementDir) * PLAYER_SPEED * dt;
                }
            }
        }

        networkClient.pollReceive();
        
        WorldSnapshot &worldSnapshot = networkClient.getWorldSnapshot();
        if (worldSnapshot.appear) {
            worldSnapshot.appear = false;
            entityManager.applySnapshot(worldSnapshot, networkClient.getClientId(), pendingInputs);
        }

        ChunkSnapshot &chunkSnapshot = networkClient.getChunkSnapshot();
        if (chunkSnapshot.appear) {
            chunkSnapshot.appear = false;
            debugRenderer.applySnapshot(chunkSnapshot);
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
            entityManager.update(CLIENT_TICK, networkClient.getClientId());    
        }

        window.clear(sf::Color(30.0f, 30.0f, 30.0f));
        
        renderer.render(entityManager, networkClient.getClientId());

        if (debugRenderer.isEnabled() || true) { // for quick test
            debugRenderer.render(entityManager, worldCollision);
        }

        renderer.renderUI(inventory, equipment);

        window.display();

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}