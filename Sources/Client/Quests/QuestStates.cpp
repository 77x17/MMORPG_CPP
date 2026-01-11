#include "Client/Quests/QuestStates.hpp"

#include "Client/Snapshots/QuestSnapshot.hpp"

void QuestStates::applySnapshot(QuestSnapshot &questSnapshot) {
    quests.clear();
    for (QuestStateSnapshot &questSnapshot : questSnapshot.quests) {
        QuestState questState;
        questState.questId = questSnapshot.questId;
        questState.status  = static_cast<QuestStatus>(questSnapshot.status);
        for (ObjectiveStateSnapshot &objectiveSnapshot : questSnapshot.objectives) {
            ObjectiveState objectiveState;
            objectiveState.type      = static_cast<ObjectiveType>(objectiveSnapshot.type);
            objectiveState.current   = objectiveSnapshot.current;
            objectiveState.completed = objectiveSnapshot.completed;
            questState.objectives.push_back(objectiveState);
        }
        quests.push_back(questState);
    }
}

std::vector<QuestState> & QuestStates::getQuests() {
    return quests;
}