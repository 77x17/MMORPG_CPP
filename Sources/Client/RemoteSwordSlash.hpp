#pragma once

#include <SFML/System/Vector2.hpp>

struct RemoteSwordSlash {
    int          id = -1;
    sf::Vector2f serverPosition{ 0.0f, 0.0f };
    sf::Vector2f localPosition { 0.0f, 0.0f };
    sf::Vector2f size          { 0.0f, 0.0f };
    int          ownerId        = -1;
    bool         authoritative  = false;       // true if this client is authoritative for this projectile
};