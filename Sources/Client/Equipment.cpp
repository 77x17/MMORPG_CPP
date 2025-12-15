#include "Equipment.hpp"

Equipment::Equipment(int _size) : size(_size) {
    slots.reserve(size);
    for (int i = 0; i < size; ++i) {
        slots.push_back({ -1 });
    }
}

ItemSlot & Equipment::operator [] (size_t i) {
    return slots[i];
}

const ItemSlot & Equipment::operator [] (size_t i) const {
    return slots[i];
}

ItemSlot & Equipment::getSlot(int index) {
    return slots[index];
}

const std::vector<ItemSlot> & Equipment::getSlots() const {
    return slots;
}