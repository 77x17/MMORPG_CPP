#pragma once

#include "Client/Quests/QuestTypes.hpp"

struct ObjectiveState {
    ObjectiveType type;
    int  current   = 0;
    bool completed = false;
};