#include "Server/Systems/Interest/InterestSystem.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"

std::vector<Player *> InterestSystem::getVisiblePlayers(Player *center, const std::vector<Player *> &players) {
    std::vector<Player *> visiblePlayers;
    for (Player *player : players) {
        if (distance(center->getPosition(), player->getPosition()) <= INTEREST_RADIUS) {
            visiblePlayers.push_back(player);
        }
    }
    return visiblePlayers;
}

std::vector<DamageEntity *> InterestSystem::getVisibleDamageEntities(Player *center, const std::vector<DamageEntity *> damageEntities) {
    std::vector<DamageEntity *> visibleDamageEntities;
    for (DamageEntity *damageEntity : damageEntities) {
        if (distance(center->getPosition(), damageEntity->getPosition()) <= INTEREST_RADIUS || damageEntity->getOwnerId() == center->getId()) {
            visibleDamageEntities.push_back(damageEntity);
        }
    }
    return visibleDamageEntities;
}