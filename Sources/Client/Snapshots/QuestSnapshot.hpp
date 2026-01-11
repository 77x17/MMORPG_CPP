#pragma once

#include <cstdint>
#include <vector>

struct ObjectiveStateSnapshot {
    uint8_t type;
    int     current;
    bool    completed;
};

struct QuestStateSnapshot {
    uint8_t questId;
    uint8_t status;
    std::vector<ObjectiveStateSnapshot> objectives;
};

struct QuestSnapshot {
    bool appear = false;

    std::vector<QuestStateSnapshot> quests;
};