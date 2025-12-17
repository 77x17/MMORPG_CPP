#pragma once

#include <vector>

struct InventorySnapshot {
    bool appear = false;
    std::vector<int> itemIds;
};