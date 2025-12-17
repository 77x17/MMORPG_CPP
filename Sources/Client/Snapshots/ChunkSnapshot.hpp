#pragma once

#include <vector>

#include "Shared/AABB.hpp"

struct ChunkSnapshot {
    bool appear = false;
    std::vector<AABB> chunks;
};