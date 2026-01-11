#pragma once

#include "Server/Quests/QuestObjective.hpp"

class KillObjective : public QuestObjective {
private:
    int enemyId;
    int count;

public:
    KillObjective(int _enemyId, int _count);

    ObjectiveType getType() const override;
     
    bool isCompleted(const Player &player, const ObjectiveState &state) const override;
    
    void onEvent(const Player &player, ObjectiveState &state, int killedEnemyId) const override;

    std::string getProgressText(const ObjectiveState &state) const override;
};