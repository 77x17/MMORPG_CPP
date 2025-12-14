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

#include <iostream>

int main() {
    NetworkClient networkClient("127.0.0.1", 55001, 55002);
    if (!networkClient.connectAll()) return -1;

    EntityManager entityManager;
    Inventory inventory;

    sf::RenderWindow window({ 800, 600 }, "Client");
    Renderer renderer(window);

    int myId = -1;
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
            }
            else if (renderer.getInventoryUI().isOpen()) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                        sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
                        renderer.getInventoryUI().handleLeftClick(worldPosition, inventory);
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
                    std::pair<int, int> swapItemEvent = renderer.getInventoryUI().handleRelease(worldPosition);

                    if (swapItemEvent.first != -1 && swapItemEvent.second != -1) {
                        // std::swap(inventory.getSlot(swapItemEvent.first), inventory.getSlot(swapItemEvent.second));
                        sf::Packet moveItemPacket;
                        moveItemPacket << "MoveItem" << swapItemEvent.first << swapItemEvent.second;
                        networkClient.sendTcpPacket(moveItemPacket);
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
            
                if (entityManager.getPlayers().count(myId) > 0) {
                    RemotePlayer &localPlayer = entityManager.getPlayer(myId);
                    localPlayer.localPosition += normalize(input.movementDir) * PLAYER_SPEED * dt;
                }
            }
        }

        networkClient.pollReceive();
        WorldSnapshot &worldSnapshot = networkClient.getWorldSnapshot();
        if (worldSnapshot.appear) {
            if (networkClient.assignedId != -1) myId = networkClient.assignedId;
            entityManager.applySnapshot(worldSnapshot, myId, pendingInputs);
            worldSnapshot.appear = false;
        }
        InventorySnapshot &inventorySnapshot = networkClient.getInventorySnapshot();
        if (inventorySnapshot.appear) {
            if ((int)inventory.getSlots().size() != (int)inventorySnapshot.itemIds.size()) {
                std::cout << "[Client] - InventorySnapshot bug!\n";
                return -1;
            }
            for (int index = 0; index < (int)inventorySnapshot.itemIds.size(); ++index) {
                inventory.getSlot(index).itemId = inventorySnapshot.itemIds[index];
            }
            inventorySnapshot.appear = false;
        }

        entityManager.update(dt, myId);
        
        renderer.render(entityManager, inventory, myId);

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}