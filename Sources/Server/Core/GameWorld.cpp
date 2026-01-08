#include "Server/Core/GameWorld.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp" 

#include "Server/Systems/Inventory/InventoryManager.hpp"

GameWorld::GameWorld() {}

void GameWorld::addEnemy(int enemyId) {
    int entityId = entityIdGenerator.next();
    Enemy *newEnemy = new Enemy(entityId, enemyId, sf::Vector2f(300, 200));
    
    enemies.push_back(newEnemy);
    chunkSystem.addEnemy(newEnemy);
}

void GameWorld::removeEnemy(int entityId) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->getEntityId() == entityId) {
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

int GameWorld::addPlayer(int clientId) {
    int entityId = entityIdGenerator.next();

    Player *newPlayer = new Player(entityId, clientId, sf::Vector2f(500, 500));

    if (not InventoryManager::loadInventory(newPlayer->getClientId(), newPlayer->getInventory())) {
        InventoryManager::saveInventory(newPlayer->getClientId(), newPlayer->getInventory());
    }
    if (not InventoryManager::loadEquipment(newPlayer->getClientId(), newPlayer->getEquipment())) {
        InventoryManager::saveEquipment(newPlayer->getClientId(), newPlayer->getEquipment());
    }
    
    players.push_back(newPlayer);
    chunkSystem.addPlayer(newPlayer);

    return entityId;
}

void GameWorld::removePlayer(int clientId) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i]->getClientId() == clientId) {
            InventoryManager::saveInventory(players[i]->getClientId(), players[i]->getInventory());
            InventoryManager::saveEquipment(players[i]->getClientId(), players[i]->getEquipment());
            
            chunkSystem.removePlayer(players[i]);
            chunkSystem.removeClientTracking(players[i]->getClientId());

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
    for (Player *player : players) {
        if (player == nullptr) continue;
        if (player->isDestroyed()) continue;
        
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
        if (player->getClientId() == clientId) return player;
    }
    return nullptr;
}

const Player * GameWorld::getPlayer(int clientId) const {
    for (Player *player : players) if (player != nullptr) {
        if (player->getClientId() == clientId) return player;
    }
    return nullptr;
}

int GameWorld::generateEntityId() {
    return entityIdGenerator.next();
}

Player * GameWorld::getPlayerWithEntityId(int entityId) {
    for (Player *player : players) {
        if (player == nullptr) continue;

        if (player->getEntityId() == entityId) return player;
    }
    return nullptr;
}

const Enemy * GameWorld::getEnemy(int entityId) const {
    for (Enemy *enemy : enemies) {
        if (enemy == nullptr) continue;

        if (enemy->getEntityId() == entityId) return enemy;
    }
    return nullptr;
}