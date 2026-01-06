#include "Server/Core/Chunk/ChunkSystem.hpp"

#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"
#include "Server/Entities/Enemy.hpp"

#include <iostream>

ChunkCoord ChunkSystem::getChunk(const sf::Vector2f& pos) const {
    return {
        static_cast<int>(pos.x) / CHUNK_SIZE,
        static_cast<int>(pos.y) / CHUNK_SIZE
    };
}

std::vector<ChunkCoord> ChunkSystem::getChunkInRange(int clientId, const sf::Vector2f& position, int range) {
    std::vector<ChunkCoord> result;
    ChunkCoord center = getChunk(position);

    if (center == previousChunk[clientId]) {
        return std::vector<ChunkCoord>();
    }
    previousChunk[clientId] = center;

    for (int dx = -range; dx <= range; ++dx) {
        for (int dy = -range; dy <= range; ++dy) {
            ChunkCoord chunk( center.getX() + dx, center.getY() + dy );
            result.push_back(chunk);
        }
    }
    return result;
}

void ChunkSystem::removeClientTracking(int clientId) {
    previousChunk.erase(clientId);
}

void ChunkSystem::addPlayer(Player* player) {
    if (player == nullptr) return;

    ChunkCoord chunk = getChunk(player->getPosition());
    playerChunks[chunk].insert(player);

    playerToChunk[player] = chunk;
}

void ChunkSystem::removePlayer(Player* player) {
    if (player == nullptr) return;
    
    auto it = playerToChunk.find(player);
    if (it == playerToChunk.end()) return;

    ChunkCoord chunk = it->second; 

    auto chunkIt = playerChunks.find(chunk);
    if (chunkIt != playerChunks.end()) {
        chunkIt->second.erase(player);
        if (chunkIt->second.empty()) {
            playerChunks.erase(chunkIt);
        }
    }
    playerToChunk.erase(it);
}

void ChunkSystem::updatePlayer(Player* player, const sf::Vector2f& oldPosition) {
    if (player == nullptr) return;

    auto it = playerToChunk.find(player);
    if (it == playerToChunk.end()) return;

    ChunkCoord oldChunk = it->second;
    ChunkCoord newChunk = getChunk(player->getPosition());


    if (oldChunk != newChunk) {
        playerChunks[oldChunk].erase(player);
        if (playerChunks[oldChunk].empty()) {
            playerChunks.erase(oldChunk);
        }

        playerChunks[newChunk].insert(player);
        it->second = newChunk;
    }
}

std::vector<Player*> ChunkSystem::getPlayersInRange(const sf::Vector2f& position, int range) const {
    std::vector<Player*> result;
    ChunkCoord center = getChunk(position);

    for (int dx = -range; dx <= range; ++dx) {
        for (int dy = -range; dy <= range; ++dy) {
            ChunkCoord chunk( center.getX() + dx, center.getY() + dy );
            auto it = playerChunks.find(chunk);
            if (it != playerChunks.end()) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        }
    }
    return result;
}

void ChunkSystem::addDamageEntity(DamageEntity* entity) {
    if (entity == nullptr) return;
    
    ChunkCoord chunk = getChunk(entity->getPosition());
    damageEntityChunks[chunk].insert(entity);

    damageEntityToChunk[entity] = chunk;
}

void ChunkSystem::removeDamageEntity(DamageEntity* entity) {
    if (entity == nullptr) return;

    auto it = damageEntityToChunk.find(entity);
    if (it == damageEntityToChunk.end()) return;

    ChunkCoord chunk = it->second;

    auto chunkIt = damageEntityChunks.find(chunk);
    if (chunkIt != damageEntityChunks.end()) {
        chunkIt->second.erase(entity);
        if (chunkIt->second.empty()) {
            damageEntityChunks.erase(chunkIt);
        }
    }

    damageEntityToChunk.erase(it);
}

void ChunkSystem::updateDamageEntity(DamageEntity* entity, const sf::Vector2f& oldPosition) {
    if (entity == nullptr) return;

    auto it = damageEntityToChunk.find(entity);
    if (it == damageEntityToChunk.end()) return;

    ChunkCoord oldChunk = it->second;
    ChunkCoord newChunk = getChunk(entity->getPosition());

    if (oldChunk != newChunk) {
        damageEntityChunks[oldChunk].erase(entity);
        if (damageEntityChunks[oldChunk].empty()) {
            damageEntityChunks.erase(oldChunk);
        }

        damageEntityChunks[newChunk].insert(entity);
        it->second = newChunk;
    }
}

std::vector<DamageEntity*> ChunkSystem::getDamageEntitiesInRange(const sf::Vector2f& position, int range) const {
    std::vector<DamageEntity*> result;
    ChunkCoord center = getChunk(position);

    for (int dx = -range; dx <= range; ++dx) {
        for (int dy = -range; dy <= range; ++dy) {
            ChunkCoord chunk( center.getX() + dx, center.getY() + dy );
            auto it = damageEntityChunks.find(chunk);
            if (it != damageEntityChunks.end()) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        }
    }
    return result;
}

void ChunkSystem::addEnemy(Enemy* enemy) {
    if (enemy == nullptr) return;

    ChunkCoord chunk = getChunk(enemy->getPosition());
    enemyChunks[chunk].insert(enemy);

    enemyToChunk[enemy] = chunk;
}

void ChunkSystem::removeEnemy(Enemy* enemy) {
    if (enemy == nullptr) return;

    auto it = enemyToChunk.find(enemy);
    if (it == enemyToChunk.end()) return;

    ChunkCoord chunk = it->second;

    auto chunkIt = enemyChunks.find(chunk);
    if (chunkIt != enemyChunks.end()) {
        chunkIt->second.erase(enemy);
        if (chunkIt->second.empty()) {
            enemyChunks.erase(chunkIt);
        }
    }
    enemyToChunk.erase(it);
}

void ChunkSystem::updateEnemy(Enemy* enemy, const sf::Vector2f& oldPosition) {
    if (enemy == nullptr) return;

    auto it = enemyToChunk.find(enemy);
    if (it == enemyToChunk.end()) return;

    ChunkCoord oldChunk = it->second;
    ChunkCoord newChunk = getChunk(enemy->getPosition());

    if (oldChunk != newChunk) {
        enemyChunks[oldChunk].erase(enemy);
        if (enemyChunks[oldChunk].empty()) {
            enemyChunks.erase(oldChunk);
        }

        enemyChunks[newChunk].insert(enemy);
        it->second = newChunk;
    }
}

std::vector<Enemy*> ChunkSystem::getEnemiesInRange(const sf::Vector2f& position, int range) const {
    std::vector<Enemy*> result;
    ChunkCoord center = getChunk(position);

    for (int dx = -range; dx <= range; ++dx) {
        for (int dy = -range; dy <= range; ++dy) {
            ChunkCoord chunk( center.getX() + dx, center.getY() + dy );
            auto it = enemyChunks.find(chunk);
            if (it != enemyChunks.end()) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        }
    }
    return result;
}