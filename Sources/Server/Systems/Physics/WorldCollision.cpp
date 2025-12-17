#include "Server/Systems/Physics/WorldCollision.hpp"

void WorldCollision::addStaticCollider(const AABB& box) {
    staticCollisions.push_back(box);
}

bool WorldCollision::checkCollision(const AABB &box) const {
    for (const AABB& collider : staticCollisions) {
        if (box.intersect(collider)) {
            return true;
        }
    }
    return false;
}

const std::vector<AABB> & WorldCollision::getColliders() const {
    return staticCollisions;
}