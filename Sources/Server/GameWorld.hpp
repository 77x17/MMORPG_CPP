#pragma once

#include <vector>

#include "Player.hpp"
#include "DamageEntity.hpp"

class GameWorld {
private:
    std::vector<Player *> players;
    
    int nextProjectileId;
    std::vector<DamageEntity *> damageEntities;

public:
    void addPlayer(int id);
    void removePlayer(int id);

    void addDamageEntity(DamageEntity *newDamageEntity);

    void update(const float &dt);

    const std::vector<Player *> getPlayers() const;
    const std::vector<DamageEntity *> getDamageEntities() const;
};