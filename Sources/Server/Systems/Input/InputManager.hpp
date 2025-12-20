#pragma once

#include <unordered_map>
#include <vector>

#include "Shared/InputState.hpp"

class InputManager {
private:
    std::unordered_map<int, std::vector<InputState>> clientQueues;
    std::unordered_map<int, std::vector<InputState>> enemyQueues;

public:
    void pushClientInput(int clientId, const InputState &input);
    void pushEnemyInput(int enemyId, const InputState &input);

    std::vector<InputState> & getClientQueue(int clientId);
    std::vector<InputState> & getEnemyQueue(int enemyId);

    void clearClientQueue(int clientId);
    void clearEnemyQueue(int enemyId);
};