#pragma once

#include "Server/Quests/QuestTypes.hpp"

struct ObjectiveState {
    ObjectiveType type;
    int  current   = 0;
    int  required  = 0;
    bool completed = false;
};