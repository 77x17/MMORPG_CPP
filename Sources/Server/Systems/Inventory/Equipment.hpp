#pragma once

#include <vector>
#include "Server/Systems/Inventory/ItemSlot.hpp"

class Equipment {
private:
    int size = 8;
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

    ItemSlot & operator [] (size_t index);
    const ItemSlot & operator [] (size_t index) const;

    ItemSlot & getHelmet();
    ItemSlot & getChestplate();
    ItemSlot & getLeggings();
    ItemSlot & getBoots();
    ItemSlot & getWeapon();
    ItemSlot & getRing();
    ItemSlot & getAmulet();
    ItemSlot & getShield();

    int getSize() const;
};
