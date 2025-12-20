#pragma once

#include <vector>

class Player;
class DamageEntity;
class Enemy;

constexpr float INTEREST_RADIUS = 1000.0f;

class InterestSystem {
public:
    std::vector<Player *> getVisiblePlayers(Player *center, const std::vector<Player *> &players);
    std::vector<DamageEntity *> getVisibleDamageEntities(Player *center, const std::vector<DamageEntity *> damageEntities);
    std::vector<Enemy *> getVisibleEnemies(Player *center, const std::vector<Enemy* > enemies);
};