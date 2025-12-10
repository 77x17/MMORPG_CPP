#pragma once

#include <vector>

#include "Player.hpp"
#include "DamageEntity.hpp"

constexpr float INTEREST_RADIUS = 800.0f;

class InterestSystem {
public:
    std::vector<Player *> getVisiblePlayers(Player *center, const std::vector<Player *> &players);
    std::vector<DamageEntity *> getVisibleDamageEntities(Player *center, const std::vector<DamageEntity *> damageEntities);
};