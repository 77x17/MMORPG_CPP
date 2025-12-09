#include "CombatSystem.hpp"

void CombatSystem::handleCollision(GameWorld &gameWorld) {
    const std::vector<Player *> &players = gameWorld.getPlayers();
    const std::vector<Projectile *> &projectiles = gameWorld.getProjectiles();
    
    for (Projectile *projectile : projectiles) {
        if (!projectile->isDestroyed()) {
            for (Player *player : players) if (projectile->getOwnerId() != player->getId()) {
                if (projectile->getBounds().intersects(player->getBounds())) {
                    player->takeDamage(projectile->getDamage());
                    projectile->destroy();
                    break;
                }
            }
        }
    }
}