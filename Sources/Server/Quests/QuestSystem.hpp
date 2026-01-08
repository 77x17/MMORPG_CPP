#pragma once

class Player;

#include "Server/Quests/QuestTypes.hpp"

class QuestSystem {
public:
    static void onEnemyKilled(Player *player, int enemyId);
    static void onPlayerMoved(Player *player);
    static void acceptQuest(Player *player, QuestID id);
};