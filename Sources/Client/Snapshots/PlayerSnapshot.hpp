#pragma once

#include <string>

struct PlayerSnapshot {
    int   id;
    float x, y;
    int   hp;
    int   lastProcessed;
    std::string name;
};