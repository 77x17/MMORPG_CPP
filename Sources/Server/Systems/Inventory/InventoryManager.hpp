#pragma once

class Inventory;
class Equipment;

class InventoryManager {
public:
    static bool loadInventory(int playerId, Inventory &inventory);
    static bool saveInventory(int playerId, const Inventory &inventory);

    static bool loadEquipment(int playerId, Equipment &equipment);
    static bool saveEquipment(int playerId, const Equipment &equipment);
};