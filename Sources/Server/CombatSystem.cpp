#include "CombatSystem.hpp"

void CombatSystem::handleCollision(GameWorld &gameWorld) {
    const std::vector<Player *> &players = gameWorld.getPlayers();
    const std::vector<Projectile *> &projectiles = gameWorld.getProjectiles();
    
    for (Projectile *projectile : projectiles) {
        if (!projectile->isDestroyed()) {
            for (Player *player : players) if (projectile->getOwnerId() != player->getId()) {
                if (projectile->getBounds().intersects(player->getBounds())) {
                    player->takeDamage(projectile->getDamage());
                    player->knockback(-projectile->getVelocity(), projectile->getKnockback());
                    projectile->destroy();
                    break;
                }
            }
        }
    }

    int playerSize = players.size();
    for (int i = 0; i < playerSize; ++i) {
        for (int j = i + 1; j < playerSize; ++j) {
            if (players[i]->getBounds().intersects(players[j]->getBounds())) {
                players[i]->knockback(-players[j]->getVelocity(), players[j]->getKnockback());
                players[j]->knockback(-players[i]->getVelocity(), players[i]->getKnockback());
            }
        }
    }
}