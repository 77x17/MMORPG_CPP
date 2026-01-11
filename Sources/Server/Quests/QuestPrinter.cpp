#include "Server/Quests/QuestPrinter.hpp"

#include "Server/Quests/PlayerQuestState.hpp"
#include "Server/Quests/QuestManager.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

void QuestPrinter::printQuest(const std::vector<PlayerQuestState> &questStates) {
    for (const PlayerQuestState &questState : questStates) {
        LogSystem::addMessage("[Quests] " + std::to_string(questState.questId) + ((questState.status == QuestStatus::InProgress) ? ": In Progress" : "Completed"));
        
        const QuestDefinition *questDefinition = QuestManager::instance().getQuest(questState.questId);
        if (questDefinition == nullptr) continue;

        LogSystem::addMessage("[Quests] " + questDefinition->name + ": " + questDefinition->description);

        for (size_t i = 0; i < questDefinition->objectives.size(); ++i) {
            LogSystem::addMessage("[Quests] " + questDefinition->objectives[i]->getProgressText(questState.objectives[i]));
        }
    }
}