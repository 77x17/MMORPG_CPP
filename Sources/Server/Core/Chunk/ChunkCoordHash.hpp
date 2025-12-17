#pragma once

#include <cstddef>
#include <functional>

#include "Server/Core/Chunk/ChunkCoord.hpp"

struct ChunkCoordHash {
    std::size_t operator()(const ChunkCoord& c) const {
        return std::hash<int>()(c.getX()) ^ (std::hash<int>()(c.getY()) << 1);
    }
};