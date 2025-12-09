#include "InputManager.hpp"

#include <SFML/Window/Keyboard.hpp>
#include "../Shared/Utils.hpp"

bool InputManager::getPlayerInput(InputState &state) {
    state.movementDir = { 0.f, 0.f };
    state.isShooting  = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) state.movementDir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) state.movementDir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) state.movementDir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) state.movementDir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) state.isShooting = true;
    
    state.movementDir = normalize(state.movementDir);
    return (state.movementDir.x != 0.f || state.movementDir.y != 0.f || state.isShooting);
}