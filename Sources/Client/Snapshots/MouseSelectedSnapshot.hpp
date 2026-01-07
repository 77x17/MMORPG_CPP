#pragma once

#include <string>

struct MouseSelectedSnapshot {
    bool appear = false;

    std::string name;
    int entityId;
    int enemyId;
    int hp;
    int maxHp;
};