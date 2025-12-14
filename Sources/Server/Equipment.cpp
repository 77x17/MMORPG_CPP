#include "Equipment.hpp"

Equipment::Equipment(int _size) : size(_size) {
    slots.assign(size, ItemSlot());
}

std::vector<ItemSlot> & Equipment::getSlots() {
    return slots;
}