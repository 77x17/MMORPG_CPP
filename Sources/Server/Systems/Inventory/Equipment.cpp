#include "Server/Systems/Inventory/Equipment.hpp"

#include <stdexcept>

Equipment::Equipment() {   
    helmet     = ItemSlot();
    chestplate = ItemSlot();
    leggings   = ItemSlot();
    boots      = ItemSlot();
    weapon     = ItemSlot();
    ring       = ItemSlot();
    amulet     = ItemSlot();
    shield     = ItemSlot();
}

ItemSlot & Equipment::operator [] (size_t index) {
    switch (index) {
        case 0: return helmet;
        case 1: return chestplate;
        case 2: return leggings;
        case 3: return boots;
        case 4: return weapon;
        case 5: return ring;
        case 6: return amulet;
        case 7: return shield;
    }
    throw std::out_of_range("[Equipment] - Index out of bounds");
}

const ItemSlot & Equipment::operator [] (size_t index) const {
    switch (index) {
        case 0: return helmet;
        case 1: return chestplate;
        case 2: return leggings;
        case 3: return boots;
        case 4: return weapon;
        case 5: return ring;
        case 6: return amulet;
        case 7: return shield;
    }
    throw std::out_of_range("[Equipment] - Index out of bounds");
}

ItemSlot & Equipment::getHelmet() {
    return helmet;
}

ItemSlot & Equipment::getChestplate() {
    return chestplate;
}

ItemSlot & Equipment::getLeggings() {
    return leggings;
}

ItemSlot & Equipment::getBoots() {
    return boots;
}

ItemSlot & Equipment::getWeapon() {
    return weapon;
}

ItemSlot & Equipment::getRing() {
    return ring;
}

ItemSlot & Equipment::getAmulet() {
    return amulet;
}

ItemSlot & Equipment::getShield() {
    return shield;
}

int Equipment::getSize() const {
    return size;
}