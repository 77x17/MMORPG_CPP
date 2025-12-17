#pragma once

class GameWorld;

class InventorySystem {
private:
    GameWorld &gameWorld;

public:
    InventorySystem(GameWorld &_gameWorld);

    bool moveItem(int clientId, int from, int to);
    bool equipItem(int clientId, int fromInventory, int toEquipment);
};