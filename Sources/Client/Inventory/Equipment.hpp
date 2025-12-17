#pragma once

#include <vector>
#include "Client/Inventory/ItemSlot.hpp"

class Equipment {
private:
    int size;
    std::vector<ItemSlot> slots;

public:
    // helmet, chestplate, leggings, boots, weapon, ring, amulet, shield
    // 0     , 1         , 2       , 3    , 4     , 5   , 6     , 7
    Equipment(int size = 8);

    ItemSlot & operator [] (size_t i);
    const ItemSlot & operator [] (size_t i) const;

    ItemSlot &getSlot(int index);
    const std::vector<ItemSlot> & getSlots() const;
};