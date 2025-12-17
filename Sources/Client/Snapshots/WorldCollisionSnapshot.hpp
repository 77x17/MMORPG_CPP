#pragma once

#include <vector>

#include "Shared/AABB.hpp"

struct WorldCollisionSnapshot {
    bool appear = false;
    std::vector<AABB> colliders;
};