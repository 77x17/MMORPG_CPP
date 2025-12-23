#include "Client/World/WorldCollision.hpp"

void WorldCollision::setColliders(const std::vector<AABB> &_colliders) {
    colliders = _colliders;
}

const std::vector<AABB> & WorldCollision::getColliders() const {
    return colliders;
}

std::vector<AABB> & WorldCollision::getColliders() {
    return colliders;
}