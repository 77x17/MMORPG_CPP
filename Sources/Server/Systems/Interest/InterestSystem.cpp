#include "Server/Systems/Interest/InterestSystem.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"

std::vector<Player *> InterestSystem::getVisiblePlayers(Player *center, const std::vector<Player *> &players) {
    std::vector<Player *> visiblePlayers;
    for (Player *player : players) {
        if (player == nullptr) continue;
        
        if (distance(center->getPosition(), player->getPosition()) <= INTEREST_RADIUS) {
            visiblePlayers.push_back(player);
        }
    }
    return visiblePlayers;
}

std::vector<DamageEntity *> InterestSystem::getVisibleDamageEntities(Player *center, const std::vector<DamageEntity *> damageEntities) {
    std::vector<DamageEntity *> visibleDamageEntities;
    for (DamageEntity *damageEntity : damageEntities) {
        if (damageEntity == nullptr) continue;

        if (distance(center->getPosition(), damageEntity->getPosition()) <= INTEREST_RADIUS || damageEntity->getOwnerEntityId() == center->getEntityId()) {
            visibleDamageEntities.push_back(damageEntity);
        }
    }
    return visibleDamageEntities;
}

std::vector<Enemy *> InterestSystem::getVisibleEnemies(Player *center, const std::vector<Enemy* > enemies) {
    std::vector<Enemy *> visibleEnemies;
    for (Enemy *enemy : enemies) if (enemy != nullptr) {
        if (distance(center->getPosition(), enemy->getPosition()) <= INTEREST_RADIUS) {
            visibleEnemies.push_back(enemy);
        }
    }
    return visibleEnemies;
}