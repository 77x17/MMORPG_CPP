#pragma once

#include <cstdint>

using QuestID = uint8_t;

enum class QuestStatus {
    InProgress,
    Completed,
    Rewarded
};

enum class ObjectiveType {
    Kill,
    ReachLocation
};