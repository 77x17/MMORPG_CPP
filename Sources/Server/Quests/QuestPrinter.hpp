#pragma once

#include <vector>

class PlayerQuestState;

class QuestPrinter {
public:
    static void printQuest(const std::vector<PlayerQuestState> &questStates);
};