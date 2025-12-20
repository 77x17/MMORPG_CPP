#pragma once

#include <vector>

#include <SFML/System/Vector2.hpp>

class WorldCollision;
class Enemy;
class Player;
class InputState;

class PhysicsSystem {
private:
    WorldCollision &worldCollision;

private:
    void moveWithCollision(Player &player, const sf::Vector2f &delta);
    void moveWithCollision(Enemy  &enemy , const sf::Vector2f &delta);

public:
    PhysicsSystem(WorldCollision &_worldCollision);

    void updatePlayer(Player &player, const InputState &inputState, const float &dt);
    void updateEnemy(Enemy &enemy, const InputState &inputState, const float &dt);

    void resolvePlayerCollisions(const std::vector<Player *> &players);
    void resolveEnemyCollisions(const std::vector<Enemy *> &enemies);
    void resolvePlayerWithEnemyCollisions(const std::vector<Player *> &players, const std::vector<Enemy *> &enemies);
};