#include "Server/Systems/Combat/CombatSystem.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Enemy.hpp"
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
        for (Player *player : players) if (player != nullptr) {
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

void CombatSystem::handleCollision(const std::vector<Enemy *> &enemies, const std::vector<DamageEntity *> &damageEntities) {
    for (DamageEntity *damageEntity : damageEntities) {
        if (damageEntity->isDestroyed() || damageEntity->getDamage() == 0) {
            continue;
        }

        bool hitSomething = false;
        for (Enemy *enemy : enemies) if (enemy != nullptr) {
            if (damageEntity->getOwnerId() == enemy->getId()) {
                continue;
            }

            if (not damageEntity->getBounds().intersects(enemy->getBounds())) {
                continue;
            }

            damageEntity->onHit(*enemy);
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