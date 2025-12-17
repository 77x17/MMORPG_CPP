#pragma once

#include "Shared/InputState.hpp"

struct NewInputEvent {
    int        clientId;
    InputState input;
};