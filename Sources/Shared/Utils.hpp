#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

constexpr float EPSILON = 0.001;

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

inline float dist2(const sf::Vector2f &a, const sf::Vector2f &b) {
    sf::Vector2f d = a - b; 
    return d.x * d.x + d.y * d.y;
};

inline float distance(const sf::Vector2f &a, const sf::Vector2f &b) {
    return sqrt(dist2(a, b));
}
