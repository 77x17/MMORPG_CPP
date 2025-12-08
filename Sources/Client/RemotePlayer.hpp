#pragma once

#include <SFML/System.hpp>

#include "../Shared/InterfaceEntity.hpp"

struct RemotePlayer {
    int          id = -1;
    sf::Vector2f serverPosition{ 0.0f, 0.0f };
    sf::Vector2f serverVelocity{ 0.0f, 0.0f };
    sf::Vector2f localPosition { 0.0f, 0.0f };
    int          hp              = 0;
    int          lastAck         = 0;       // Last processed input seq from server
    double       serverTimestamp = 0.0;     // timestamp of the server snapshot (for interpolation)
};