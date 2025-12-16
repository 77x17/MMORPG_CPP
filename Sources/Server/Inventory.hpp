#pragma once

#include <vector>

#include "ItemSlot.hpp"

class Inventory {
private:
    int size = 36;
    std::vector<ItemSlot> slots;

public:
    Inventory();

    ItemSlot & operator [] (size_t i) {
        return slots[i];
    }

    const ItemSlot & operator [] (size_t i) const {
        return slots[i];
    }

    std::vector<ItemSlot> & getSlots();

};