#pragma once

#include <vector>
#include "ItemSlot.hpp"

class Equipment {
private:
    ItemSlot helmet;
    ItemSlot chestplate;
    ItemSlot leggings;
    ItemSlot boots;
    ItemSlot weapon;
    ItemSlot ring;
    ItemSlot amulet;
    ItemSlot shield;

public:
    Equipment();

    ItemSlot & getHelmet();
    ItemSlot & getChestplate();
    ItemSlot & getLeggings();
    ItemSlot & getBoots();
    ItemSlot & getWeapon();
    ItemSlot & getRing();
    ItemSlot & getAmulet();
    ItemSlot & getShield();
};
