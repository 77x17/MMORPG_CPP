#pragma once

#include <vector>

class Player;
class DamageEntity;

class CombatSystem {
public:
    void handleCollision(const std::vector<Player *> &players, const std::vector<DamageEntity *> &damageEntities);
};