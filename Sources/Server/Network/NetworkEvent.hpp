#pragma once

#include "Shared/InputState.hpp"

enum class NetworkEventType {
    NewClient,
    Disconnect,
    Input,
    MoveItem,
    EquipItem
};

struct NetworkEvent {
    NetworkEventType type;
    
    // === Payload ===
    int clientId = -1;

    InputState input;
    
    int from = -1;
    int to   = -1;
};



