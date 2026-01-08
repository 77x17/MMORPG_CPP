#pragma once

#include "Server/Entities/EntityType.hpp"

struct KilledEvent {
    int fromEntityId;
    int toEntityId;
    EntityType fromType;
    EntityType toType;
};