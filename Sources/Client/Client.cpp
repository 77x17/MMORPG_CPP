#include <SFML/Window/Event.hpp>

#include <SFML/System/Sleep.hpp>

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
                        renderer.getInventoryUI().handleLeftClick(worldPosition);
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
                    std::pair<int, int> swapItemEvent = renderer.getInventoryUI().handleRelease(worldPosition);

                    if (swapItemEvent.first != -1 && swapItemEvent.second != -1) {
                        std::swap(inventory.getSlot(swapItemEvent.first), inventory.getSlot(swapItemEvent.second));
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
                    // localPlayer.localPosition += normalize(input.movementDir) * PLAYER_SPEED * SERVER_TICK;
                    localPlayer.localPosition += normalize(input.movementDir) * PLAYER_SPEED * dt;
                }
            }
        }

        auto maybeSnap = networkClient.pollReceive();
        if (maybeSnap) {
            if (networkClient.assignedId != -1) myId = networkClient.assignedId;
            entityManager.applySnapshot(*maybeSnap, myId, pendingInputs);
        }

        entityManager.update(dt, myId);
        
        renderer.render(entityManager, inventory, myId);

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}