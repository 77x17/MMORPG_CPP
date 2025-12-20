#pragma once

#include <SFML/System/Vector2.hpp>

struct RemoteEnemy {
    int id = -1;
    sf::Vector2f serverPosition{ 0.0f, 0.0f };
    sf::Vector2f serverVelocity{ 0.0f, 0.0f };
    sf::Vector2f localPosition { 0.0f, 0.0f };
    int hp;
};