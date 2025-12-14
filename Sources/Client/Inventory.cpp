#include "Inventory.hpp"

Inventory::Inventory(int _size) : size(_size) {
    slots.reserve(size);
    for (int i = 0; i < size; ++i) {
        slots.push_back({ i });
    }
}

InventorySlot & Inventory::getSlot(int index) {
    return slots[index];
}

const std::vector<InventorySlot> & Inventory::getSlots() const {
    return slots;
}