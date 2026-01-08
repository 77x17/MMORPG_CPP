#pragma once

#include <vector>

#include "Server/Quests/QuestTypes.hpp"
#include "Server/Quests/ObjectiveState.hpp"

struct PlayerQuestState {
    QuestID questId;
    QuestStatus status = QuestStatus::InProgress;
    std::vector<ObjectiveState> objectives;
};