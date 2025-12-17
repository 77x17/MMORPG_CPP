#include "Server/Core/GameWorld.hpp"

#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp" 

#include "Server/Systems/Inventory/InventoryManager.hpp"

GameWorld::GameWorld() {}

#include <iostream>

void GameWorld::addPlayer(int id) {
    Player *newPlayer = new Player(id, sf::Vector2f(500, 500));

    if (not InventoryManager::loadInventory(newPlayer->getId(), newPlayer->getInventory())) {
        InventoryManager::saveInventory(newPlayer->getId(), newPlayer->getInventory());
    }
    if (not InventoryManager::loadEquipment(newPlayer->getId(), newPlayer->getEquipment())) {
        InventoryManager::saveEquipment(newPlayer->getId(), newPlayer->getEquipment());
    }
    
    players.push_back(newPlayer);
    chunkSystem.addPlayer(newPlayer);
}

void GameWorld::removePlayer(int id) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i]->getId() == id) {
            InventoryManager::saveInventory(players[i]->getId(), players[i]->getInventory());
            InventoryManager::saveEquipment(players[i]->getId(), players[i]->getEquipment());
            
            chunkSystem.removePlayer(players[i]);
            delete players[i];
            players.erase(players.begin() + i);

            break;
        }
    }
}

void GameWorld::addDamageEntity(DamageEntity *newDamageEntity) {
    damageEntities.push_back(newDamageEntity);
    chunkSystem.addDamageEntity(newDamageEntity);
}

void GameWorld::update(const float &dt) {
    for (Player* player : players) {
        player->update(dt);
        chunkSystem.updatePlayer(player, player->getOldPosition());
    }

    for (DamageEntity* damageEntity : damageEntities) {
        damageEntity->update(dt);
        chunkSystem.updateDamageEntity(damageEntity, damageEntity->getOldPosition());
    }

    for (size_t i = 0; i < damageEntities.size(); ++i) {
        if (damageEntities[i]->isDestroyed()) {
            chunkSystem.removeDamageEntity(damageEntities[i]);
            delete damageEntities[i];
            damageEntities.erase(damageEntities.begin() + i);
            --i;
        }
    }
}

const std::vector<Player *> GameWorld::getPlayers() const {
    return players;
}

const std::vector<DamageEntity *> GameWorld::getDamageEntities() const {
    return damageEntities;
}

const std::vector<Player *> GameWorld::getPlayersInChunk(const sf::Vector2f &centerPosition) const {
    return chunkSystem.getPlayersInRange(centerPosition);
}

const std::vector<DamageEntity *> GameWorld::getDamageEntitiesInChunk(const sf::Vector2f &centerPosition) const {
    return chunkSystem.getDamageEntitiesInRange(centerPosition);
}

std::vector<ChunkCoord> GameWorld::getChunkInRange(int clientId, const sf::Vector2f &centerPosition) {
    return chunkSystem.getChunkInRange(clientId, centerPosition);
}

Player * GameWorld::getPlayer(int clientId) {
    for (Player *player : players) {
        if (player->getId() == clientId) return player;
    }
    return nullptr;
}