#pragma once

#include <vector>

#include "Client/Snapshots/EnemySnapshot.hpp"
#include "Client/Snapshots/PlayerSnapshot.hpp"
#include "Client/Snapshots/ProjectileSnapshot.hpp"
#include "Client/Snapshots/SwordSlashSnapshot.hpp"

struct WorldSnapshot {
    bool appear = false;
    std::vector<EnemySnapshot> enemies;
    std::vector<PlayerSnapshot> players;
    std::vector<ProjectileSnapshot> projectiles;
    std::vector<SwordSlashSnapshot> swordSlashs;
};