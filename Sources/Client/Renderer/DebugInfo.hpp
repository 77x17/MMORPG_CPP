#pragma once

#include <SFML/System/Vector2.hpp>

struct DebugInfo {
    sf::Vector2f playerPosition;
    float        fps  = 0.0f;
    int          tcpPing = 0;
    int          udpPing = 0;
};