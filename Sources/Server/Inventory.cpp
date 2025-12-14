#include "Inventory.hpp"

Inventory::Inventory(int _size) : size(_size) {
    slots.assign(size, ItemSlot());
    slots[0].id = 0;
    slots[1].id = 1;
}

std::vector<ItemSlot> & Inventory::getSlots() {
    return slots;
}