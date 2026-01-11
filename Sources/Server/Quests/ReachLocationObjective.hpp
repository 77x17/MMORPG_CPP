#pragma once

#include "Server/Quests/QuestObjective.hpp"

class ReachLocationObjective : public QuestObjective {
private:
    float x, y;
    float radius;

public:
    ReachLocationObjective(float _x, float _y, float _radius);

    ObjectiveType getType() const override;

    bool isCompleted(const Player &player, const ObjectiveState &state) const override;

    void onEvent(const Player &player, ObjectiveState &state, int /* unused */) const override;

    std::string getProgressText(const ObjectiveState &state) const override;
};