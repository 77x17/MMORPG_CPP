#include "Server/Systems/Combat/CombatSystem.hpp"

#include "Shared/Utils.hpp"

#include "Server/Core/GameWorld.hpp"

#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"
#include "Server/Entities/Projectile.hpp"
#include "Server/Entities/SwordSlash.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

#include "Server/Quests/QuestSystem.hpp"

void CombatSystem::handleCollision(const std::vector<Player *> &players, const std::vector<DamageEntity *> &damageEntities) {
    for (DamageEntity *damageEntity : damageEntities) {
        if (damageEntity->isDestroyed() || damageEntity->getDamage() == 0) {
            continue;
        }

        bool hitSomething = false;
        for (Player *player : players) {
            if (player == nullptr) continue;
            if (player->isDestroyed()) continue;

            if (damageEntity->getOwnerEntityId() == player->getEntityId()) {
                continue;
            }

            if (not damageEntity->getBounds().intersects(player->getBounds())) {
                continue;
            }

            damageEntity->onHit(*player);

            if (player->isDestroyed()) {
                killedEvents.push_back({
                    damageEntity->getOwnerEntityId(),
                    player->getEntityId(),
                    damageEntity->getOwnerType(),
                    EntityType::Player
                });
            }

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
        for (Enemy *enemy : enemies) {
            if (enemy == nullptr) continue;
            if (enemy->isDestroyed()) continue;

            if (damageEntity->getOwnerEntityId() == enemy->getEntityId()) {
                continue;
            }

            if (not damageEntity->getBounds().intersects(enemy->getBounds())) {
                continue;
            }

            damageEntity->onHit(*enemy);
            
            if (enemy->isDestroyed()) {
                killedEvents.push_back({ 
                    damageEntity->getOwnerEntityId(), 
                    enemy->getEntityId(),
                    damageEntity->getOwnerType(),
                    EntityType::Enemy
                });
            }

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

void CombatSystem::handleCollision(const std::vector<Player *> &players, const std::vector<Enemy *> &enemies, const std::vector<DamageEntity *> &damageEntities) {
    handleCollision(players, damageEntities);
    handleCollision(enemies, damageEntities);
}

void CombatSystem::handleKilledEvents(GameWorld &gameWorld) {
    for (KilledEvent &event : killedEvents) {
        if (event.fromType == EntityType::Player) {
            Player *player = gameWorld.getPlayerWithEntityId(event.fromEntityId);
            if (player == nullptr) continue;

            if (event.toType == EntityType::Player) {
                const Player *kPlayer = gameWorld.getPlayerWithEntityId(event.toEntityId);
                if (kPlayer == nullptr) continue; 

                LogSystem::addMessage("[Combat] Player " + std::to_string(player->getClientId()) + " killed Player " + std::to_string(kPlayer->getClientId()));
            }
            else if (event.toType == EntityType::Enemy) {
                const Enemy *enemy = gameWorld.getEnemy(event.toEntityId);
                if (enemy == nullptr) continue;
                
                LogSystem::addMessage("[Combat] Player " + std::to_string(player->getClientId()) + " killed Enemy " + std::to_string(enemy->getEnemyId()));
            
                QuestSystem::onEnemyKilled(player, enemy->getEnemyId());
            }
        }
        else if (event.fromType == EntityType::Enemy) {
            const Enemy *enemy = gameWorld.getEnemy(event.fromEntityId);
            if (enemy == nullptr) continue;

            if (event.toType == EntityType::Player) {
                const Player *player = gameWorld.getPlayerWithEntityId(event.toEntityId);
                if (player == nullptr) continue; 

                LogSystem::addMessage("[Combat] Enemy " + std::to_string(enemy->getEnemyId()) + " killed Player " + std::to_string(player->getClientId()));
            }
            else if (event.toType == EntityType::Enemy) {
                const Enemy *kEnemy = gameWorld.getEnemy(event.toEntityId);
                if (kEnemy == nullptr) continue;
                
                LogSystem::addMessage("[Combat] Enemy " + std::to_string(enemy->getEnemyId()) + " killed Enemy " + std::to_string(kEnemy->getEnemyId()));
            }
        }
    }

    killedEvents.clear();
}