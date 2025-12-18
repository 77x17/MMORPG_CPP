#include "Server/Systems/Input/InputManager.hpp"

void InputManager::pushInput(int clientId, const InputState &input) {
    inputQueues[clientId].push_back(input);
}

std::vector<InputState> & InputManager::getQueue(int clientId) {
    return inputQueues[clientId];
}

void InputManager::clearQueue(int clientId) {
    inputQueues[clientId].clear();
}