#pragma once

#include <vector>

class WorldCollision;
class Player;
class InputState;

class PhysicsSystem {
private:
    WorldCollision &worldCollision;

public:
    PhysicsSystem(WorldCollision &_worldCollision);

    void updatePlayer(Player &player, const InputState &inputState, const float &dt);

    void resolvePlayerCollisions(const std::vector<Player *> &players);
};