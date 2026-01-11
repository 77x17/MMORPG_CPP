#pragma once

#include <vector>

#include "Client/Quests/QuestTypes.hpp"
#include "Client/Quests/ObjectiveState.hpp"

struct QuestSnapshot;

struct QuestState {
    QuestID questId;
    QuestStatus status;
    std::vector<ObjectiveState> objectives;
};

class QuestStates {
private:
    std::vector<QuestState> quests;

public:
    QuestStates() = default;

    void applySnapshot(QuestSnapshot &questSnapshot);

    std::vector<QuestState> & getQuests();
};