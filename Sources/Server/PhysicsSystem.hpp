#pragma once

#include <vector>

#include <SFML/System/Vector2.hpp>

class WorldCollision;
class Player;
class InputState;

class PhysicsSystem {
private:
    WorldCollision &worldCollision;

private:
    void moveWithCollision(Player &player, const sf::Vector2f &delta);

public:
    PhysicsSystem(WorldCollision &_worldCollision);

    void updatePlayer(Player &player, const InputState &inputState, const float &dt);

    void resolvePlayerCollisions(const std::vector<Player *> &players);
};