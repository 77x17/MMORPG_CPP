#include "Server/Quests/QuestSystem.hpp"

#include "Server/Entities/Player.hpp"

#include "Server/Quests/QuestManager.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

#include <iostream>

void QuestSystem::onEnemyKilled(Player *player, int enemyId) {
    for (PlayerQuestState & playerQuest : player->getQuests()) {
        if (playerQuest.status != QuestStatus::InProgress) continue;

        const QuestDefinition *quest = QuestManager::instance().getQuest(playerQuest.questId);
        if (quest == nullptr) continue;

        for (size_t i = 0; i < quest->objectives.size(); ++i) {
            if (quest->objectives[i]->getType() != ObjectiveType::Kill) continue;
            quest->objectives[i]->onEvent(*player, playerQuest.objectives[i], enemyId);
        }

        bool done = true;
        for (ObjectiveState &objective : playerQuest.objectives) {
            if (objective.completed == false) done = false;
        }

        if (done) {
            playerQuest.status = QuestStatus::Completed;
            LogSystem::addMessage("[Quests] Player ID " + std::to_string(player->getClientId()) + " completed Quest ID " + std::to_string(playerQuest.questId));
        }
    }
}

void QuestSystem::onPlayerMoved(Player *player) {
    if (player == nullptr) return;

    for (PlayerQuestState & playerQuest : player->getQuests()) {
        if (playerQuest.status != QuestStatus::InProgress) continue;

        const QuestDefinition *quest = QuestManager::instance().getQuest(playerQuest.questId);
        if (quest == nullptr) continue;

        for (size_t i = 0; i < quest->objectives.size(); ++i) {
            if (quest->objectives[i]->getType() != ObjectiveType::ReachLocation) continue;
            quest->objectives[i]->onEvent(*player, playerQuest.objectives[i], 0);
        }

        bool done = true;
        for (ObjectiveState &objective : playerQuest.objectives) {
            if (objective.completed == false) done = false;
        }

        if (done) {
            playerQuest.status = QuestStatus::Completed;
            LogSystem::addMessage("[Quests] Player ID " + std::to_string(player->getClientId()) + " completed Quest ID " + std::to_string(playerQuest.questId));
        }
    }
}

void QuestSystem::acceptQuest(Player *player, QuestID id) {
    if (player == nullptr) return;

    const QuestDefinition *quest = QuestManager::instance().getQuest(id);
    if (quest == nullptr) return;

    PlayerQuestState state;
    state.questId = id;
    for (auto &objective : quest->objectives) {
        ObjectiveState os;
        os.type      = objective->getType();
        os.current   = 0;
        os.required  = 0;   // optional
        os.completed = false;
        state.objectives.push_back(os);
    }

    player->getQuests().push_back(state);
}