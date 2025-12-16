#include "CombatSystem.hpp"

#include "../Shared/Utils.hpp"

#include "Player.hpp"
#include "DamageEntity.hpp"
#include "Projectile.hpp"
#include "SwordSlash.hpp"

void CombatSystem::handleCollision(const std::vector<Player *> &players, const std::vector<DamageEntity *> &damageEntities) {
    for (DamageEntity *damageEntity : damageEntities) {
        if (damageEntity->isDestroyed() || damageEntity->getDamage() == 0) {
            continue;
        }

        bool hitSomething = false;
        for (Player *player : players) {
            if (damageEntity->getOwnerId() == player->getId()) {
                continue;
            }

            if (not damageEntity->getBounds().intersects(player->getBounds())) {
                continue;
            }

            damageEntity->onHit(*player);
            hitSomething = true;

            if (!damageEntity->canHitMultiple()) {
                damageEntity->destroy();
                break;
            }
        }
        
        if (hitSomething && damageEntity->canHitMultiple()) {
            damageEntity->destroy();
        }
    }
}