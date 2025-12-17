#include "Server/Systems/Input/InputSystem.hpp"

void InputSystem::pushInput(int clientId, const InputState &input) {
    inputQueues[clientId].push_back(input);
}

std::vector<InputState> & InputSystem::getQueue(int clientId) {
    return inputQueues[clientId];
}

void InputSystem::clearQueue(int clientId) {
    inputQueues[clientId].clear();
}