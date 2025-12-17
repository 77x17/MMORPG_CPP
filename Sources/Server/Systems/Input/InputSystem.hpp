#pragma once

#include <unordered_map>
#include <vector>

#include "Shared/InputState.hpp"

class InputSystem {
private:
    std::unordered_map<int, std::vector<InputState>> inputQueues;

public:
    void pushInput(int clientId, const InputState &input);

    std::vector<InputState> & getQueue(int clientId);

    void clearQueue(int clientId);
};