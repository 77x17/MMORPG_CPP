#include "Server/Core/Chunk/ChunkCoord.hpp"

ChunkCoord::ChunkCoord(int _x, int _y) 
: x(_x), y(_y) {

}

bool ChunkCoord::operator == (const ChunkCoord &other) const {
    return x == other.x && y == other.y;
}

ChunkCoord & ChunkCoord::operator = (const ChunkCoord &other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
    }

    return *this;
}