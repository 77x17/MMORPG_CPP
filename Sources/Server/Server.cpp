#include "NetworkServer.hpp"
#include "InputSystem.hpp"
#include "CombatSystem.hpp"
#include "InterestSystem.hpp"
#include "GameWorld.hpp"
#include "Projectile.hpp"
#include "SwordSlash.hpp"
#include "WorldCollision.hpp"
#include "PhysicsSystem.hpp"
#include "WeaponSystem.hpp"

constexpr float SEND_HZ      = 30.0f;
constexpr float SEND_INTEVAL = 1.0f / SEND_HZ;
constexpr float SERVER_HZ    = 60.0f;
constexpr float SERVER_TICK  = 1.0f / SERVER_HZ;

#include <iostream>

// === For server ===
void syncGameWorldFromClients(GameWorld &gameWorld, InputSystem &inputSystem, NetworkServer &networkServer, WorldCollision &worldCollision);
void update(GameWorld &gameWorld, InputSystem &inputSystem, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem, CombatSystem &combatSystem);
void loadCollisions(WorldCollision &worldCollision);
// === For client ===
void syncInventoryToClient(GameWorld &gameWorld, NetworkServer &networkServer, int clientId);
void syncEquipmentToClient(GameWorld &gameWorld, NetworkServer &networkServer, int clientId);
void syncGameWorldToClients(GameWorld &gameWorld, InterestSystem &interestSystem, NetworkServer &networkServer);
void sendWorldCollision(WorldCollision &worldCollision, NetworkServer &networkServer, int clientId);

void syncGameWorldFromClients(GameWorld &gameWorld, InputSystem &inputSystem, NetworkServer &networkServer, WorldCollision &worldCollision) {
    std::vector<NewClientEvent> & newClientEvents = networkServer.fetchNewClients();
    if (newClientEvents.size()) {
        for (NewClientEvent &event : newClientEvents) {
            gameWorld.addPlayer(event.clientId);
            inputSystem.clearQueue(event.clientId);

            syncInventoryToClient(gameWorld, networkServer, event.clientId);
            syncEquipmentToClient(gameWorld, networkServer, event.clientId);

            sendWorldCollision(worldCollision, networkServer, event.clientId);
        }
        newClientEvents.clear();
    }

    std::vector<NewInputEvent> & newInputEvents = networkServer.fetchInputs();
    if (newInputEvents.size()) {
        for (NewInputEvent &event : newInputEvents) {
            inputSystem.pushInput(event.clientId, event.input);
        }
        newInputEvents.clear();
    }

    std::vector<DeleteClientEvent> & deleteClientEvents = networkServer.fetchDeleteClients();
    if (deleteClientEvents.size()) {
        for (DeleteClientEvent &event : deleteClientEvents) {
            gameWorld.removePlayer(event.clientId);
        }
        deleteClientEvents.clear();
    }

    std::vector<MoveItemEvent> & moveItemEvents = networkServer.fetchMoveItems();
    if (moveItemEvents.size()) {
        for (MoveItemEvent &event : moveItemEvents) {
            if (gameWorld.moveItem(event.clientId, event.from, event.to)) {
                syncInventoryToClient(gameWorld, networkServer, event.clientId);
            }
        }
        moveItemEvents.clear();
    }

    std::vector<EquipItemEvent> & equipItemEvents = networkServer.fetchEquipItems();
    if (equipItemEvents.size()) {
        for (EquipItemEvent &event : equipItemEvents) {
            if (gameWorld.equipItem(event.clientId, event.fromInventory, event.toEquipment)) {
                syncInventoryToClient(gameWorld, networkServer, event.clientId);
                syncEquipmentToClient(gameWorld, networkServer, event.clientId);
            }
        }
        equipItemEvents.clear();
    }
}

int nextProjectileId = 0;

void update(GameWorld &gameWorld, InputSystem &inputSystem, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem, CombatSystem &combatSystem) {
    for (Player *player : gameWorld.getPlayers()) {
        InputState latestInput;
        std::vector<InputState> &inputQueue = inputSystem.getQueue(player->getId());

        if (!inputQueue.empty()) {
            latestInput = inputQueue.back();
            inputSystem.clearQueue(player->getId());
            player->lastProcessedInput = latestInput.seq;
        }

        physicsSystem.updatePlayer(*player, latestInput, SERVER_TICK);

        if (DamageEntity *damageEntity = weaponSystem.tryFire(*player, latestInput)) {
            damageEntity->setId(++nextProjectileId);
            gameWorld.addDamageEntity(damageEntity);
        }
    }

    physicsSystem.resolvePlayerCollisions(gameWorld.getPlayers());
    combatSystem.handleCollision(gameWorld.getPlayers(), gameWorld.getDamageEntities());

    gameWorld.update(SERVER_TICK);
}

void syncInventoryToClient(GameWorld &gameWorld, NetworkServer &networkServer, int clientId) {
    Inventory &inventory = gameWorld.getPlayer(clientId)->getInventory();
    sf::Packet inventoryPacket;
    inventoryPacket << "Inventory" << (int)inventory.getSlots().size();
    for (const ItemSlot &slot : inventory.getSlots()) {
        inventoryPacket << slot.id;
    }
    networkServer.sendToClientTcp(clientId, inventoryPacket);
}

void syncEquipmentToClient(GameWorld &gameWorld, NetworkServer &networkServer, int clientId) {
    Equipment &equipment = gameWorld.getPlayer(clientId)->getEquipment();
    sf::Packet equipmentPacket;
    equipmentPacket << "Equipment" << equipment.getSize();
    for (int index = 0; index < equipment.getSize(); ++index) {
        equipmentPacket << equipment[index].id;
    }
    networkServer.sendToClientTcp(clientId, equipmentPacket);
}

void syncGameWorldToClients(GameWorld &gameWorld, InterestSystem &interestSystem, NetworkServer &networkServer) {
    for (ClientSession &client : networkServer.getClients()) {
        if (client.udpPort == 0) continue;

        Player *currentPlayer = nullptr;
        for (Player *player : gameWorld.getPlayers()) {
            if (player->getId() == client.id) currentPlayer = player;
        }
        sf::Packet worldStatePacket;
        worldStatePacket << "WorldState";
        
        std::vector<Player *> visiblePlayers = interestSystem.getVisiblePlayers(currentPlayer, gameWorld.getPlayers());
        worldStatePacket << (int)visiblePlayers.size();
        for (Player *player : visiblePlayers) {
            worldStatePacket << player->getId() 
                                << player->getPosition().x 
                                << player->getPosition().y 
                                << player->getHealth() 
                                << player->lastProcessedInput;
        }

        std::vector<DamageEntity *> visibleDamageEntities = interestSystem.getVisibleDamageEntities(currentPlayer, gameWorld.getDamageEntities());
        worldStatePacket << (int)visibleDamageEntities.size();
        for (DamageEntity *damageEntity : visibleDamageEntities) {
            Projectile *projectile = dynamic_cast<Projectile *>(damageEntity);
            if (projectile) {
                worldStatePacket << "Projectile"
                                    << projectile->getId() 
                                    << projectile->getPosition().x 
                                    << projectile->getPosition().y 
                                    << projectile->getVelocity().x
                                    << projectile->getVelocity().y
                                    << projectile->getOwnerId();
            }

            SwordSlash *swordSlash = dynamic_cast<SwordSlash *>(damageEntity);
            if (swordSlash) {
                worldStatePacket << "SwordSlash"
                                    << swordSlash->getId()
                                    << swordSlash->getBounds().left
                                    << swordSlash->getBounds().top
                                    << swordSlash->getBounds().width
                                    << swordSlash->getBounds().height
                                    << swordSlash->getOwnerId();
            }
        }

        networkServer.sendToClientUdp(client, worldStatePacket);
    }
}

void loadCollisions(WorldCollision &worldCollision) {
    worldCollision.addStaticCollider({
        { 300.0f, 300.0f },
        { 100.0f, 100.0f }
    });
    worldCollision.addStaticCollider({
        { 300.0f, 400.0f },
        { 100.0f, 100.0f }
    });
    worldCollision.addStaticCollider({
        { 450.0f, 300.0f },
        { 100.0f, 100.0f }
    });
    worldCollision.addStaticCollider({
        { 200.0f, 500.0f },
        { 100.0f, 100.0f }
    });
}

void sendWorldCollision(WorldCollision &worldCollision, NetworkServer &networkServer, int clientId) {
    sf::Packet worldCollisionPacket;
    worldCollisionPacket << "WorldCollision";

    const std::vector<AABB> &colliders = worldCollision.getColliders();
    worldCollisionPacket << static_cast<int>(colliders.size());
    for (const AABB &box : colliders) {
        worldCollisionPacket << box.position.x << box.position.y << box.size.x << box.size.y;
    }

    networkServer.sendToClientTcp(clientId, worldCollisionPacket);
}

int main() {
    NetworkServer  networkServer;
    if (!networkServer.start(55001, 55002)) {
        return -1;
    }
   
    GameWorld      gameWorld;

    InputSystem    inputSystem;
    WorldCollision worldCollision;
    PhysicsSystem  physicsSystem(worldCollision);
    WeaponSystem   weaponSystem;
    CombatSystem   combatSystem;
    InterestSystem interestSystem;
    
    loadCollisions(worldCollision);

    float accumulator = 0.0f;
   
    sf::Clock clock, sendClock;
    while (true) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        networkServer.poll();

        syncGameWorldFromClients(gameWorld, inputSystem, networkServer, worldCollision);

        if (accumulator >= SERVER_TICK) {
            update(gameWorld, inputSystem, physicsSystem, weaponSystem, combatSystem);
            
            accumulator -= SERVER_TICK;
        }

        if (sendClock.getElapsedTime().asSeconds() >= SEND_INTEVAL) {
            syncGameWorldToClients(gameWorld, interestSystem, networkServer);

            sendClock.restart();
        }

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}