#pragma once

#include <SFML/System/Clock.hpp>

class FPSCounter {
private:
    sf::Clock clock;
    int frames = 0;
    float fps = 0.0f;

public:
    void update();
    int getFPS() const;
};
