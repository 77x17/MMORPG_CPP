#pragma once

#include <vector>

#include "../Shared/Player.hpp"
#include "../Shared/Projectile.hpp"

constexpr float INTEREST_RADIUS = 800.0f;

class InterestSystem {
public:
    std::vector<Player *>     getVisiblePlayers(Player *center, const std::vector<Player *> &players);
    std::vector<Projectile *> getVisibleProjectiles(Player *center, const std::vector<Projectile *> projectiles);
};