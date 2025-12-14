#include "Equipment.hpp"

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
