#pragma once

#include <vector>

#include "../Shared/AABB.hpp"

class WorldCollision {
private:
    std::vector<AABB> colliders;

public:
    void setColliders(const std::vector<AABB> &_colliders);
    const std::vector<AABB> & getColliders() const;
    std::vector<AABB> & getColliders();

};