#include "Server/Systems/Physics/WorldCollisionSystem.hpp"

#include "Server/Systems/Physics/WorldCollision.hpp"

WorldCollisionSystem::WorldCollisionSystem(WorldCollision &_worldCollision) 
: worldCollision(_worldCollision) {

}

void WorldCollisionSystem::loadStaticCollisions() {
    worldCollision.addStaticCollider({
        { 300.0f, 300.0f },
        { 100.0f, 100.0f }
    });
    worldCollision.addStaticCollider({
        { 300.0f, 400.0f },
        { 100.0f, 100.0f }
    });
    worldCollision.addStaticCollider({
        { 450.0f, 300.0f },
        { 100.0f, 100.0f }
    });
    worldCollision.addStaticCollider({
        { 200.0f, 500.0f },
        { 100.0f, 100.0f }
    });
}