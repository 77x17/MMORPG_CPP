#pragma once

#include <vector>

#include "Player.hpp"
#include "Projectile.hpp"

class GameWorld {
private:
    std::vector<Player *> players;
    
    int nextProjectileId;
    std::vector<Projectile *> projectiles;

public:
    void addPlayer(int id);
    void removePlayer(int id);

    void addProjectile(Projectile *newProjectile);

    void update(const float &dt);

    const std::vector<Player *> getPlayers() const;
    const std::vector<Projectile *> getProjectiles() const;
};