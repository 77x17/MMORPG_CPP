#pragma once

#include <vector>

#include "PlayerSnapshot.hpp"
#include "ProjectileSnapshot.hpp"
#include "SwordSlashSnapshot.hpp"

struct WorldSnapshot {
    std::vector<PlayerSnapshot> players;
    std::vector<ProjectileSnapshot> projectiles;
    std::vector<SwordSlashSnapshot> swordSlashs;
    int lastAssignedId = -1;
};