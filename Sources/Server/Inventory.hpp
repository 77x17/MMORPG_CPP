#pragma once

#include <vector>

#include "InventorySlot.hpp"

class Inventory {
private:
    int size;
    std::vector<InventorySlot> slots;

public:
    Inventory(int _size = 36);

    std::vector<InventorySlot> & getSlots();

};