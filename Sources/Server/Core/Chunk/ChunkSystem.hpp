#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SFML/System/Vector2.hpp>

#include "Server/Core/Chunk/ChunkCoord.hpp"
#include "Server/Core/Chunk/ChunkCoordHash.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"

constexpr int CHUNK_RANGE = 2;

class ChunkSystem {
private:
    std::unordered_map<int, ChunkCoord> previousChunk;
    std::unordered_map<ChunkCoord, std::unordered_set<Player *>, ChunkCoordHash> playerChunks;
    std::unordered_map<ChunkCoord, std::unordered_set<DamageEntity *>, ChunkCoordHash> damageEntityChunks;

private:
    ChunkCoord getChunk(const sf::Vector2f &position) const;

public:
    ChunkSystem() = default;
    
    void addPlayer(Player *player);
    void removePlayer(Player *player);
    void updatePlayer(Player *player, const sf::Vector2f &oldPosition);

    void addDamageEntity(DamageEntity* entity);
    void removeDamageEntity(DamageEntity* entity);
    void updateDamageEntity(DamageEntity* entity, const sf::Vector2f& oldPosition);

    std::vector<Player*> getPlayersInRange(const sf::Vector2f& position, int chunkRange = CHUNK_RANGE) const;
    std::vector<DamageEntity*> getDamageEntitiesInRange(const sf::Vector2f& position, int chunkRange = CHUNK_RANGE) const;
    std::vector<ChunkCoord> getChunkInRange(int clientId, const sf::Vector2f& position, int chunkRange = CHUNK_RANGE);
};