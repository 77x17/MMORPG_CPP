#include "Inventory.hpp"

Inventory::Inventory(int _size) : size(_size) {
    slots.reserve(size);
    for (int i = 0; i < size; ++i) {
        slots.push_back({ -1 });
    }
}

ItemSlot & Inventory::operator [] (size_t i) {
    return slots[i];
}

const ItemSlot & Inventory::operator [] (size_t i) const {
    return slots[i];
}

ItemSlot & Inventory::getSlot(int index) {
    return slots[index];
}

const std::vector<ItemSlot> & Inventory::getSlots() const {
    return slots;
}