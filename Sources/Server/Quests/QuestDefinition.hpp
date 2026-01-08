#pragma once

#include "Server/Quests/QuestObjective.hpp"
#include <vector>
#include <memory>

struct QuestDefinition {
    QuestID id;
    std::string name;
    std::string description;

    std::vector<std::unique_ptr<QuestObjective>> objectives;
};