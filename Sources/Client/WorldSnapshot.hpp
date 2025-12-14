#pragma once

#include <vector>

#include "PlayerSnapshot.hpp"
#include "ProjectileSnapshot.hpp"
#include "SwordSlashSnapshot.hpp"

struct WorldSnapshot {
    bool appear = false;
    std::vector<PlayerSnapshot> players;
    std::vector<ProjectileSnapshot> projectiles;
    std::vector<SwordSlashSnapshot> swordSlashs;
};