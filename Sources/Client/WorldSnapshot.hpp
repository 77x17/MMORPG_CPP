#pragma once

#include <vector>

#include "PlayerSnapshot.hpp"
#include "ProjectileSnapshot.hpp"

struct WorldSnapshot {
    std::vector<PlayerSnapshot> players;
    std::vector<ProjectileSnapshot> projectiles;
    int lastAssignedId = -1;
};