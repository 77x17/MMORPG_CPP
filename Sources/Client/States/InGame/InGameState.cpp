#include "Client/States/InGame/InGameState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Network/Packet.hpp>

#include "Shared/Constants.hpp"
#include "Shared/Utils.hpp"

#include "Client/Input/InputManager.hpp"

#include "Client/Network/NetworkClient.hpp"

#include "Client/Utils/Constants.hpp"

#include <iostream>

InGameState::InGameState(sf::RenderWindow &_window, NetworkClient &_networkClient) 
:   window(_window), networkClient(_networkClient), renderer(window), debugRenderer(window) {
    window.setTitle("[Client] - ID: " + std::to_string(networkClient.getClientId()));
}

void InGameState::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::E) {
            renderer.getInventoryUI().toggle();
        }
        else if (event.key.code == sf::Keyboard::F11) {
            renderer.setCamera();
            debugRenderer.setCamera();
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

void InGameState::update(float dt) {
    clientAccumulator += dt;

    if (window.hasFocus() && not renderer.getInventoryUI().isOpen()) {
        InputState input;
        if (InputManager::getPlayerInput(input)) {
            input.seq = ++inputSeq;
            pendingInputs.push_back(input);

            networkClient.sendInputPacket(input.seq, input.movementDir, input.isShooting);
        
            RemotePlayer *localPlayer = entityManager.getPlayer(networkClient.getClientId());
            if (localPlayer != nullptr) {
                localPlayer->localPosition += normalize(input.movementDir) * PLAYER_SPEED * dt;
            }
        }
    }

    networkClient.poll();

    // networkClient.pollTCP();
    // networkClient.pollUDP();
    
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
        }
        for (int index = 0; index < (int)equipmentSnapshot.itemIds.size(); ++index) {
            equipment[index].itemId = equipmentSnapshot.itemIds[index];
        }
    }

    WorldCollisionSnapshot &worldCollisionSnapshot = networkClient.getWorldCollisionSnapshot();
    if (worldCollisionSnapshot.appear) {
        worldCollisionSnapshot.appear = false;
        worldCollision.getColliders().clear();
        for (const AABB &box : worldCollisionSnapshot.colliders) {
            worldCollision.getColliders().push_back(box);
        }
    }

    while (clientAccumulator >= CLIENT_TICK) {
        clientAccumulator -= CLIENT_TICK;
        entityManager.update(CLIENT_TICK, networkClient.getClientId());    
    }

    fpsCounter.update();
    networkClient.update(dt);

    RemotePlayer *player = entityManager.getPlayer(networkClient.getClientId());
    if (player != nullptr) {
        debugInfo.playerPosition = player->localPosition;
    }
    debugInfo.fps     = fpsCounter.getFPS();
    debugInfo.tcpPing = networkClient.getTcpPing();
    debugInfo.udpPing = networkClient.getUdpPing();
}

void InGameState::render(sf::RenderWindow &window) {
    window.clear(sf::Color(30.0f, 30.0f, 30.0f));
        
    renderer.render(entityManager, networkClient.getClientId());

    // if (debugRenderer.isEnabled()) {
        debugRenderer.render(worldCollision, debugInfo);
    // }

    renderer.renderUI(inventory, equipment);

    window.display();
}
