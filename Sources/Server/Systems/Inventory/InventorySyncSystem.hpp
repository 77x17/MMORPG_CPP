#pragma once

class GameWorld;
class NetworkServer;

class InventorySyncSystem {
private:
    GameWorld     &gameWorld;
    NetworkServer &networkServer;

public:
    InventorySyncSystem(GameWorld &_gameWorld, NetworkServer &_networkServer);

    void syncInventoryToClient(int clientId);
    void syncEquipmentToClient(int clientId);
};