#include "Server/Systems/Inventory/InventorySyncSystem.hpp"

#include "Server/Core/GameWorld.hpp"

#include "Server/Entities/Player.hpp"

#include "Server/Network/NetworkServer.hpp"

#include "Shared/TcpPacketType.hpp"

InventorySyncSystem::InventorySyncSystem(GameWorld &_gameWorld, NetworkServer &_networkServer) 
: gameWorld(_gameWorld), networkServer(_networkServer) {
    
}

void InventorySyncSystem::syncInventoryToClient(int clientId) {
    Player *player = gameWorld.getPlayer(clientId);
    if (player == nullptr) return;

    Inventory &inventory = player->getInventory();
    sf::Packet inventoryPacket;
    inventoryPacket << static_cast<uint8_t>(TcpPacketType::Inventory) << (int)inventory.getSlots().size();
    for (const ItemSlot &slot : inventory.getSlots()) {
        inventoryPacket << slot.id;
    }
    networkServer.sendToClientTcp(clientId, inventoryPacket);
}

void InventorySyncSystem::syncEquipmentToClient(int clientId) {
    Player *player = gameWorld.getPlayer(clientId);
    if (player == nullptr) return;

    Equipment &equipment = player->getEquipment();
    sf::Packet equipmentPacket;
    equipmentPacket << static_cast<uint8_t>(TcpPacketType::Equipment) << equipment.getSize();
    for (int index = 0; index < equipment.getSize(); ++index) {
        equipmentPacket << equipment[index].id;
    }
    networkServer.sendToClientTcp(clientId, equipmentPacket);
}
