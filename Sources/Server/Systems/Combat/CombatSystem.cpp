#include "Server/Systems/Combat/CombatSystem.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"
#include "Server/Entities/Projectile.hpp"
#include "Server/Entities/SwordSlash.hpp"

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