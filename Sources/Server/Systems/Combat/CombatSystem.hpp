#pragma once

#include <vector>

class Enemy;
class Player;
class DamageEntity;

class CombatSystem {
public:
    void handleCollision(const std::vector<Player *> &players, const std::vector<DamageEntity *> &damageEntities);
    void handleCollision(const std::vector<Enemy *>  &enemies, const std::vector<DamageEntity *> &damageEntities);
};