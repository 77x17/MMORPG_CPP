#pragma once

class Player;

class InventorySystem {
public:
    InventorySystem() = default;

    bool moveItem(Player *player, int from, int to);
    bool equipItem(Player *player, int fromInventory, int toEquipment);
};