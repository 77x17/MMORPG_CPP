#include "Server/Systems/Inventory/Inventory.hpp"

Inventory::Inventory() {
    slots.assign(size, ItemSlot());
}

std::vector<ItemSlot> & Inventory::getSlots() {
    return slots;
}