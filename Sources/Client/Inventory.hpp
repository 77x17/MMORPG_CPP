#pragma once

#include <vector>
#include "InventorySlot.hpp"

class Inventory {
private:
    std::vector<InventorySlot> slots;

public:
    Inventory(int size = 20);

    InventorySlot &getSlot(int index);
    const std::vector<InventorySlot> & getSlots() const;
};