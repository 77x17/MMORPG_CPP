#pragma once

#include <vector>

#include "../Shared/AABB.hpp"

class WorldCollision {
private:
    std::vector<AABB> staticCollisions;

public:
    void addStaticCollider(const AABB& box);
    bool checkCollision(const AABB &box) const;
};