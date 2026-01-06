#include "Server/Core/GameWorld.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp" 

#include "Server/Events/EventBus.hpp"

#include "Server/Systems/Inventory/InventoryManager.hpp"

GameWorld::GameWorld(EventBus &_eventBus) 
: eventBus(_eventBus) {

}

void GameWorld::addEnemy(int id) {
    Enemy *newEnemy = new Enemy(id, sf::Vector2f(300, 200));
    
    enemies.push_back(newEnemy);
    chunkSystem.addEnemy(newEnemy);

    // eventBus.publish(EnemySpawnedEvent(newEnemy->getId()));
}

void GameWorld::removeEnemy(int id) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->getId() == id) {
            // eventBus.publish(EnemyRemovedEvent(id));

            chunkSystem.removeEnemy(enemies[i]);

            delete enemies[i];
            enemies.erase(enemies.begin() + i);

            break;
        }
    }
}

std::vector<Enemy *> & GameWorld::getEnemies() {
    return enemies;
}

const std::vector<Enemy *> & GameWorld::getEnemies() const {
    return enemies;
}

const std::vector<Enemy *> GameWorld::getEnemiesInChunk(const sf::Vector2f &centerPosition) const {
    return chunkSystem.getEnemiesInRange(centerPosition);
}

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

    // eventBus.publish(PlayerSpawnedEvent(id));
}

void GameWorld::removePlayer(int id) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i]->getId() == id) {
            InventoryManager::saveInventory(players[i]->getId(), players[i]->getInventory());
            InventoryManager::saveEquipment(players[i]->getId(), players[i]->getEquipment());
            
            chunkSystem.removePlayer(players[i]);
            chunkSystem.removeClientTracking(players[i]->getId());

            // eventBus.publish(PlayerRemovedEvent(id));

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
    for (Player *player : players) {
        player->update(dt);
        chunkSystem.updatePlayer(player, player->getOldPosition());
    }

    for (DamageEntity *damageEntity : damageEntities) {
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

    for (Enemy *enemy : enemies) {
        enemy->update(dt);
        chunkSystem.updateEnemy(enemy, enemy->getOldPosition());
    }
}

const std::vector<Player *> & GameWorld::getPlayers() const {
    return players;
}

std::vector<Player *> & GameWorld::getPlayers() {
    return players;
}

const std::vector<DamageEntity *> & GameWorld::getDamageEntities() const {
    return damageEntities;
}

const std::vector<Player *> GameWorld::getPlayersInChunk(const sf::Vector2f &centerPosition) const {
    return chunkSystem.getPlayersInRange(centerPosition);
}

Player * GameWorld::findNearestPlayer(const sf::Vector2f &position) {
    Player *nearestPlayer = nullptr;
    float nearestDistance = -1;
    for (Player *player : players) if (player != nullptr) {
        if (nearestDistance == -1) {
            nearestPlayer = player;
            nearestDistance = distance(position, player->getPosition());
            continue;
        }
        
        float currentDistance = distance(position, player->getPosition());
        if (currentDistance < nearestDistance) {
            nearestPlayer = player;
            nearestDistance = currentDistance;
        }
    }
    return nearestPlayer;
}

const std::vector<DamageEntity *> GameWorld::getDamageEntitiesInChunk(const sf::Vector2f &centerPosition) const {
    return chunkSystem.getDamageEntitiesInRange(centerPosition);
}

std::vector<ChunkCoord> GameWorld::getChunkInRange(int clientId, const sf::Vector2f &centerPosition) {
    return chunkSystem.getChunkInRange(clientId, centerPosition);
}

Player * GameWorld::getPlayer(int clientId) {
    for (Player *player : players) if (player != nullptr) {
        if (player->getId() == clientId) return player;
    }
    return nullptr;
}

const Player * GameWorld::getPlayer(int clientId) const {
    for (Player *player : players) if (player != nullptr) {
        if (player->getId() == clientId) return player;
    }
    return nullptr;
}