#pragma once

class WorldCollision;

class WorldCollisionSystem {
private:
    WorldCollision &worldCollision;

public:
    WorldCollisionSystem(WorldCollision &_worldCollision);

    void loadStaticCollisions();
};