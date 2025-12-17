#pragma once

#include <vector>

#include "Server/Core/Chunk/ChunkSystem.hpp"

class Player;
class DamageEntity;

class GameWorld {
private:
    std::vector<Player *> players;
    
    int nextProjectileId;
    std::vector<DamageEntity *> damageEntities;

    ChunkSystem chunkSystem;

public:
    GameWorld();

    void addPlayer(int id);
    void removePlayer(int id);

    void addDamageEntity(DamageEntity *newDamageEntity);

    void update(const float &dt);

    const std::vector<Player *> getPlayers() const;
    const std::vector<DamageEntity *> getDamageEntities() const;

    const std::vector<Player *> getPlayersInChunk(const sf::Vector2f &centerPosition) const;
    const std::vector<DamageEntity *> getDamageEntitiesInChunk(const sf::Vector2f &centerPosition) const;
    std::vector<ChunkCoord> getChunkInRange(int clientId, const sf::Vector2f &centerPosition);

    Player * getPlayer(int clientId);
};