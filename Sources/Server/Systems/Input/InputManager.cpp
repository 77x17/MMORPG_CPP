#include "Server/Systems/Input/InputManager.hpp"

void InputManager::pushClientInput(int clientId, const InputState &input) {
    clientQueues[clientId].push_back(input);
}

std::vector<InputState> & InputManager::getClientQueue(int clientId) {
    return clientQueues[clientId];
}

void InputManager::clearClientQueue(int clientId) {
    clientQueues[clientId].clear();
}

void InputManager::pushEnemyInput(int enemyId, const InputState &input) {
    enemyQueues[enemyId].push_back(input);
}

std::vector<InputState> & InputManager::getEnemyQueue(int enemyId) {
    return enemyQueues[enemyId];
}

void InputManager::clearEnemyQueue(int enemyId) {
    enemyQueues[enemyId].clear();
}