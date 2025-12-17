#include "Server/Systems/Inventory/InventorySystem.hpp"

#include "Server/Core/GameWorld.hpp"

#include "Server/Entities/Player.hpp"

#include "Server/Systems/Inventory/Inventory.hpp"
#include "Server/Systems/Inventory/Equipment.hpp"

InventorySystem::InventorySystem(GameWorld &_gameWorld) 
: gameWorld(_gameWorld) {

}

bool InventorySystem::moveItem(int clientId, int from, int to) {
    Player *player = gameWorld.getPlayer(clientId);
    if (player == nullptr) return false;

    Inventory &inventory = player->getInventory();
    if (inventory[from].id != -1 || inventory[to].id != -1) {
        std::swap(inventory[from].id, inventory[to].id);
        return true;
    }
    return false;
}

bool InventorySystem::equipItem(int clientId, int fromInventory, int toEquipment) {
    Player *player = gameWorld.getPlayer(clientId);
    if (player == nullptr) return false;

    Inventory &inventory = player->getInventory();
    Equipment &equipment = player->getEquipment();

    if (inventory[fromInventory].id != -1 || equipment[toEquipment].id != -1) {
        std::swap(inventory[fromInventory].id, equipment[toEquipment].id);
        return true;
    }
    return false;
}