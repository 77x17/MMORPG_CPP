#include "Server/Quests/KillObjective.hpp"

#include "Server/Quests/ObjectiveState.hpp"

#include <iostream>

KillObjective::KillObjective(int _enemyId, int _count) 
: enemyId(_enemyId), count(_count) 
{

}

ObjectiveType KillObjective::getType() const {
    return ObjectiveType::Kill;
}

bool KillObjective::isCompleted(const Player &player, const ObjectiveState &state) const {
    return state.current >= count;
}

void KillObjective::onEvent(const Player &player, ObjectiveState &state, int killedEnemyId) const {
    if (killedEnemyId == enemyId && !state.completed) {
        ++state.current;
        if (state.current >= count) {
            state.completed = true;
        }
    }
}