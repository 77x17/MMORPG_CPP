#include "Server/Quests/ReachLocationObjective.hpp"

#include "Server/Quests/ObjectiveState.hpp"

#include "Server/Entities/Player.hpp"

#include "Shared/Utils.hpp"

ReachLocationObjective::ReachLocationObjective(float _x, float _y, float _radius) 
: x(_x), y(_y), radius(_radius) 
{
    
}

ObjectiveType ReachLocationObjective::getType() const {
    return ObjectiveType::ReachLocation;
}

bool ReachLocationObjective::isCompleted(const Player &player, const ObjectiveState &state) const {
    sf::Vector2f position = player.getPosition();
    return distance(position, { x, y }) <= radius;
}

void ReachLocationObjective::onEvent(const Player &player, ObjectiveState &state, int /* unused */) const {
    if (!state.completed && isCompleted(player, state)) {
        state.completed = true;
        state.current   = 1;
    }
}

std::string ReachLocationObjective::getProgressText(const ObjectiveState &state) const {
    return state.completed ? "Reached destination" : ("Go to target location at [" + std::to_string(static_cast<int>(x)) + ", " + std::to_string(static_cast<int>(y)) + "]");
}