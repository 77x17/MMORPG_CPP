#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>

struct RemotePlayer {
    int          id = -1;
    sf::Vector2f serverPosition{ 0.0f, 0.0f };
    sf::Vector2f serverVelocity{ 0.0f, 0.0f };
    sf::Vector2f localPosition { 0.0f, 0.0f };
    int          hp              = 0;
    int          lastAck         = 0;       // Last processed input seq from server
    std::string  name;
};