#include "Server/Core/Chunk/ChunkSystem.hpp"
#include "Server/Core/Chunk/DebugChunkSyncSystem.hpp"
#include "Server/Core/GameWorld.hpp"
#include "Server/Core/GameWorldSynsSystem.hpp"

#include "Server/Entities/EnemiesManager.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/Projectile.hpp"
#include "Server/Entities/SwordSlash.hpp"

#include "Server/Network/NetworkServer.hpp"

#include "Server/Systems/AI/EnemyAISystem.hpp"
#include "Server/Systems/Combat/CombatSystem.hpp"
#include "Server/Systems/Combat/WeaponSystem.hpp"
#include "Server/Systems/Input/InputManager.hpp"
#include "Server/Systems/Input/InputSystem.hpp"
#include "Server/Systems/Interest/InterestSystem.hpp"
#include "Server/Systems/Inventory/InventorySystem.hpp"
#include "Server/Systems/Inventory/InventorySyncSystem.hpp"
#include "Server/Systems/Physics/PhysicsSystem.hpp"
#include "Server/Systems/Physics/WorldCollision.hpp"
#include "Server/Systems/Physics/WorldCollisionSystem.hpp"
#include "Server/Systems/Physics/WorldCollisionSyncSystem.hpp"

#include "Server/Utils/Constants.hpp"
#include "Server/Utils/Random.hpp"

#include <iostream>

void syncGameWorldFromClients(GameWorld &gameWorld, InputManager &inputManager, InventorySystem &inventorySystem, InventorySyncSystem &inventorySyncSystem, NetworkServer &networkServer, WorldCollisionSyncSystem &worldCollisionSyncSystem) {
    std::vector<NewClientEvent> & newClientEvents = networkServer.fetchNewClients();
    if (newClientEvents.size()) {
        for (NewClientEvent &event : newClientEvents) {
            gameWorld.addPlayer(event.clientId);
            inputManager.clearClientQueue(event.clientId);

            inventorySyncSystem.syncInventoryToClient(event.clientId);
            inventorySyncSystem.syncEquipmentToClient(event.clientId);

            worldCollisionSyncSystem.syncToClient(event.clientId);
        }
        newClientEvents.clear();
    }

    std::vector<NewInputEvent> & newInputEvents = networkServer.fetchInputs();
    if (newInputEvents.size()) {
        for (NewInputEvent &event : newInputEvents) {
            inputManager.pushClientInput(event.clientId, event.input);
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
            Player *player = gameWorld.getPlayer(event.clientId);
            if (inventorySystem.moveItem(player, event.from, event.to)) {
                inventorySyncSystem.syncInventoryToClient(event.clientId);
            }
        }
        moveItemEvents.clear();
    }

    std::vector<EquipItemEvent> & equipItemEvents = networkServer.fetchEquipItems();
    if (equipItemEvents.size()) {
        for (EquipItemEvent &event : equipItemEvents) {
            Player *player = gameWorld.getPlayer(event.clientId);
            if (inventorySystem.equipItem(player, event.fromInventory, event.toEquipment)) {
                inventorySyncSystem.syncInventoryToClient(event.clientId);
                inventorySyncSystem.syncEquipmentToClient(event.clientId);
            }
        }
        equipItemEvents.clear();
    }
}

int main() {
    NetworkServer networkServer;
    if (!networkServer.start(55001, 55002)) {
        return -1;
    }
   
    GameWorld       gameWorld;
    WorldCollision  worldCollision;
    InputManager    inputManager;
    
    EnemiesManager       enemiesManager(gameWorld);

    EnemyAISystem        enemyAISystem;
    CombatSystem         combatSystem;
    WeaponSystem         weaponSystem;
    InputSystem          inputSystem;
    InterestSystem       interestSystem;
    InventorySystem      inventorySystem;
    PhysicsSystem        physicsSystem(worldCollision);
    WorldCollisionSystem worldCollisionSystem(worldCollision);

    DebugChunkSyncSystem     debugChunkSyncSystem(gameWorld, networkServer);
    GameWorldSyncSystem      gameWorldSyncSystem(gameWorld, networkServer, interestSystem);
    InventorySyncSystem      inventorySyncSystem(gameWorld, networkServer);
    WorldCollisionSyncSystem worldCollisionSyncSystem(worldCollision, networkServer);

    uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    Random::seed(seed);

    enemiesManager.loadStaticEnemies();
    worldCollisionSystem.loadStaticCollisions();

    sf::Clock clock, sendClock;
    float accumulator = 0.0f;
    while (true) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        networkServer.poll();

        syncGameWorldFromClients(gameWorld, inputManager, inventorySystem, inventorySyncSystem, networkServer, worldCollisionSyncSystem);
    
        if (accumulator >= SERVER_TICK) {
            accumulator -= SERVER_TICK;

            enemyAISystem.update(SERVER_TICK, gameWorld, inputManager);

            inputSystem.processPlayerInputs(inputManager, gameWorld, physicsSystem, weaponSystem);
            inputSystem.processEnemyInputs(inputManager, gameWorld, physicsSystem, weaponSystem);

            physicsSystem.resolvePlayerCollisions(gameWorld.getPlayers());
            physicsSystem.resolveEnemyCollisions(gameWorld.getEnemies());
            physicsSystem.resolvePlayerWithEnemyCollisions(gameWorld.getPlayers(), gameWorld.getEnemies());

            combatSystem.handleCollision(gameWorld.getPlayers(), gameWorld.getDamageEntities());
            combatSystem.handleCollision(gameWorld.getEnemies(), gameWorld.getDamageEntities());

            gameWorld.update(SERVER_TICK);
        }
        
        if (sendClock.getElapsedTime().asSeconds() >= SEND_INTEVAL) {
            sendClock.restart();

            gameWorldSyncSystem.syncToClients();

            debugChunkSyncSystem.syncToClients();
        }

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}