#include "CombatSystem.hpp"

#include "../Shared/Utils.hpp"

#include "Projectile.hpp"
#include "SwordSlash.hpp"

void CombatSystem::handleCollision(GameWorld &gameWorld) {
    const std::vector<Player *> &players = gameWorld.getPlayers();
    const std::vector<DamageEntity *> &damageEntities = gameWorld.getDamageEntities();
    
    for (DamageEntity *damageEntity : damageEntities) {
        if (!damageEntity->isDestroyed() && damageEntity->getDamage() != 0) {
            bool destroyFlag = false;
            for (Player *player : players) if (damageEntity->getOwnerId() != player->getId()) {
                if (damageEntity->getBounds().intersects(player->getBounds())) {
                    Projectile *projectile = dynamic_cast<Projectile *>(damageEntity);
                    if (projectile) {
                        player->takeDamage(projectile->getDamage());
                        player->knockback(-projectile->getVelocity(), projectile->getKnockbackStrength());
                    }
                    
                    SwordSlash *swordSlash = dynamic_cast<SwordSlash *>(damageEntity);
                    if (swordSlash) {
                        player->takeDamage(swordSlash->getDamage());
                        
                        sf::Vector2f direction = swordSlash->getPosition() - player->getPosition();
                        player->knockback(direction, swordSlash->getKnockbackStrength());
                    }

                    if (!damageEntity->canHitMultiple()) {
                        damageEntity->destroy();
                        break;
                    }
                    else {
                        destroyFlag = true;
                    }
                }
            }
            
            if (destroyFlag) {
                damageEntity->destroy();
            }
        }
    }

    int playerSize = players.size();
    for (int i = 0; i < playerSize; ++i) {
        for (int j = i + 1; j < playerSize; ++j) {
            if (players[i]->getBounds().intersects(players[j]->getBounds())) {
                // players[i]->knockback(-players[j]->getVelocity(), players[j]->getKnockback());
                // players[j]->knockback(-players[i]->getVelocity(), players[i]->getKnockback());                
                sf::Vector2f direction = normalize(players[j]->getPosition() - players[i]->getPosition());
                if (direction.x == 0 && direction.y == 0) {
                    direction = { 1.0f, 0.0f };
                }
                
                sf::FloatRect ra = players[i]->getBounds();
                sf::FloatRect rb = players[j]->getBounds();

                float overlapX = std::min(ra.left + ra.width, rb.left + rb.width)
                           - std::max(ra.left, rb.left);
                float overlapY = std::min(ra.top + ra.height, rb.top + rb.height)
                            - std::max(ra.top, rb.top);

                float penetration = std::min(overlapX, overlapY);
                if (penetration <= 0) continue;

                sf::Vector2f push = direction * (penetration * 0.5f);
                players[i]->move(-push);
                players[j]->move( push);
            }
        }
    }
}