#pragma once

#include "Server/Quests/QuestTypes.hpp"

class Player;
struct ObjectiveState;

class QuestObjective {
public:
    virtual ~QuestObjective() = default;
    virtual ObjectiveType getType() const = 0;
    virtual bool isCompleted(const Player &player, const ObjectiveState &state) const = 0;
    virtual void onEvent(const Player &player, ObjectiveState &state, int eventParam) const = 0;
};