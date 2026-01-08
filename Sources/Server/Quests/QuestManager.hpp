#pragma once

#include <unordered_map>

#include "Server/Quests/QuestDefinition.hpp"

class QuestManager {
private:
    std::unordered_map<QuestID, QuestDefinition> quests;

public:
    static QuestManager & instance();

    const QuestDefinition * getQuest(QuestID id) const;

    bool loadFromFile(const std::string &path);
};