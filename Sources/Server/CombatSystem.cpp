#include "CombatSystem.hpp"

#include "Projectile.hpp"

void CombatSystem::handleCollision(GameWorld &gameWorld) {
    const std::vector<Player *> &players = gameWorld.getPlayers();
    const std::vector<DamageEntity *> &damageEntities = gameWorld.getDamageEntities();
    
    for (DamageEntity *damageEntity : damageEntities) {
        if (!damageEntity->isDestroyed()) {
            for (Player *player : players) if (damageEntity->getOwnerId() != player->getId()) {
                if (damageEntity->getBounds().intersects(player->getBounds())) {
                    Projectile *projectile = dynamic_cast<Projectile *>(damageEntity);
                    player->takeDamage(projectile->getDamage());
                    player->knockback(-projectile->getVelocity(), projectile->getKnockbackStrength());
                    damageEntity->destroy();
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