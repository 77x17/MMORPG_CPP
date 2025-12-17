#pragma once

#include <vector>
#include "Client/Inventory/ItemSlot.hpp"

class Inventory {
private:
    int size;
    std::vector<ItemSlot> slots;

public:
    Inventory(int size = 36);

    ItemSlot & operator [] (size_t i);
    const ItemSlot & operator [] (size_t i) const;

    ItemSlot &getSlot(int index);
    const std::vector<ItemSlot> & getSlots() const;
};