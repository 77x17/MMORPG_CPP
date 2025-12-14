#include "Inventory.hpp"

Inventory::Inventory(int _size) : size(_size) {
    slots.assign(size, InventorySlot());
}

std::vector<InventorySlot> & Inventory::getSlots() {
    return slots;
}