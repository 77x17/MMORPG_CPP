#pragma once

#include <SFML/System/Vector2.hpp>

struct InputState {
    int seq = 0;
    sf::Vector2f movementDir{0, 0};
    bool isShooting = false;
};