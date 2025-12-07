#pragma once

const float EPSILON       = 0.001;
const int   WINDOW_WIDTH  = 800;
const int   WINDOW_HEIGHT = 600;

const float SERVER_TICK   = 1.f / 60.f;

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>

inline sf::Vector2f normalize(const sf::Vector2f &source) {
    float length = std::sqrt(source.x * source.x + source.y * source.y);
    if (length > EPSILON) {
        return source / length;
    }
    else {
        return sf::Vector2f(0, 0);
    }
    return source;
}

inline sf::Vector2f lerp(const sf::Vector2f &a, const sf::Vector2f &b, const float &t) {
    return a * (1.f - t) + b * t;
}