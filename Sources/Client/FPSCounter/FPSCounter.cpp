#include "Client/FPSCounter/FPSCounter.hpp"

void FPSCounter::update() {
    frames++;

    float elapsed = clock.getElapsedTime().asSeconds();
    if (elapsed >= 1.0f) {
        fps = frames / elapsed;
        frames = 0;
        clock.restart();
    }
}

int FPSCounter::getFPS() const { 
    return static_cast<int>(fps); 
}