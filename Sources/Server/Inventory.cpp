#include "Inventory.hpp"

Inventory::Inventory(int _size) : size(_size) {
    slots.assign(size, ItemSlot());
}

std::vector<ItemSlot> & Inventory::getSlots() {
    return slots;
}