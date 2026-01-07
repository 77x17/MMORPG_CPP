#pragma once

#include <SFML/System/Vector2.hpp>

struct RemoteEnemy {
    int entityId = -1;
    int enemyId = -1;
    sf::Vector2f serverPosition{ 0.0f, 0.0f };
    sf::Vector2f serverVelocity{ 0.0f, 0.0f };
    sf::Vector2f localPosition { 0.0f, 0.0f };
    int hp;
};