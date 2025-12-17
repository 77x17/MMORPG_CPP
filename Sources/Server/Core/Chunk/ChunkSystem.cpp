#include "Server/Core/Chunk/ChunkSystem.hpp"

ChunkCoord ChunkSystem::getChunk(const sf::Vector2f& pos) const {
    return {
        static_cast<int>(pos.x) / CHUNK_SIZE,
        static_cast<int>(pos.y) / CHUNK_SIZE
    };
}

void ChunkSystem::addPlayer(Player* player) {
    ChunkCoord chunk = getChunk(player->getPosition());
    playerChunks[chunk].insert(player);
}

void ChunkSystem::removePlayer(Player* player) {
    ChunkCoord chunk = getChunk(player->getPosition());
    playerChunks[chunk].erase(player);
}

void ChunkSystem::updatePlayer(Player* player, const sf::Vector2f& oldPosition) {
    ChunkCoord oldChunk = getChunk(oldPosition);
    ChunkCoord newChunk = getChunk(player->getPosition());

    if (!(oldChunk == newChunk)) {
        playerChunks[oldChunk].erase(player);
        playerChunks[newChunk].insert(player);
    }
}

void ChunkSystem::addDamageEntity(DamageEntity* entity) {
    damageEntityChunks[getChunk(entity->getPosition())].insert(entity);
}

void ChunkSystem::removeDamageEntity(DamageEntity* entity) {
    ChunkCoord c = getChunk(entity->getPosition());
    damageEntityChunks[c].erase(entity);
}

void ChunkSystem::updateDamageEntity(DamageEntity* entity, const sf::Vector2f& oldPosition) {
    ChunkCoord oldChunk = getChunk(oldPosition);
    ChunkCoord newChunk = getChunk(entity->getPosition());

    if (!(oldChunk == newChunk)) {
        damageEntityChunks[oldChunk].erase(entity);
        damageEntityChunks[newChunk].insert(entity);
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