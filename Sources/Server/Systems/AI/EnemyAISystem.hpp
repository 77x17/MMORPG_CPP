#pragma once

#include <SFML/System/Vector2.hpp>

class GameWorld;
class InputManager;

class EnemyAISystem {
public:
    void update(const float &dt, GameWorld &gameWorld, InputManager &inputManager);
};