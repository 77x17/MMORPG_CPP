#pragma once

#include <SFML/System/Vector2.hpp>

struct AABB {
    sf::Vector2f position;
    sf::Vector2f size;

    bool intersect(const AABB &other) const {
        return position.x < other.position.x + other.size.x && 
               position.x + size.x > other.position.x       &&
               position.y < other.position.y + other.size.y &&
               position.y + size.y > other.position.y;
    }
};