#pragma once

constexpr int CHUNK_SIZE = 512;

#include <SFML/System/Vector2.hpp>

class ChunkCoord {
private:
    int x;
    int y;

public:
    ChunkCoord(int _x = 0, int _y = 0);

    int getX() const { return x; }
    int getY() const { return y; }

    bool operator == (const ChunkCoord &other) const;

    ChunkCoord & operator = (const ChunkCoord &other);
};