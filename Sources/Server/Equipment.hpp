#pragma once

#include <vector>

#include "ItemSlot.hpp"

class Equipment {
private:
    int size;
    std::vector<ItemSlot> slots;

public:
    Equipment(int _size = 8);

    std::vector<ItemSlot> & getSlots();

};