#pragma once

#include <vector>

#include "Server/Core/Chunk/ChunkSystem.hpp"

#include "Server/Entities/EntityIdGenerator.hpp"

class Enemy;
class Player;
class DamageEntity;

class EventBus;

class GameWorld {
private:
    std::vector<Enemy *> enemies;
    std::vector<Player *> players;
    std::vector<DamageEntity *> damageEntities;

    ChunkSystem chunkSystem;

    EntityIdGenerator entityIdGenerator;
    
public:
    GameWorld();

    void addEnemy();
    void removeEnemy(int entityId);
    std::vector<Enemy *> & getEnemies();
    const std::vector<Enemy *> & getEnemies() const;
    const std::vector<Enemy *> getEnemiesInChunk(const sf::Vector2f &centerPosition) const;

    int addPlayer(int clientId);
    void removePlayer(int id);
    Player * getPlayer(int clientId);
    const Player * getPlayer(int clientId) const;
    std::vector<Player *> & getPlayers();
    const std::vector<Player *> & getPlayers() const;
    const std::vector<Player *> getPlayersInChunk(const sf::Vector2f &centerPosition) const;
    Player * findNearestPlayer(const sf::Vector2f &position);

    void addDamageEntity(DamageEntity *newDamageEntity);
    const std::vector<DamageEntity *> & getDamageEntities() const;
    const std::vector<DamageEntity *> getDamageEntitiesInChunk(const sf::Vector2f &centerPosition) const;

    void update(const float &dt);

    std::vector<ChunkCoord> getChunkInRange(int clientId, const sf::Vector2f &centerPosition);
    int generateEntityId();
};