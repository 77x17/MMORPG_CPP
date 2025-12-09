#include "InterestSystem.hpp"

#include "../Shared/Utils.hpp"

std::vector<Player *> InterestSystem::getVisiblePlayers(Player *center, const std::vector<Player *> &players) {
    std::vector<Player *> visiblePlayers;
    for (Player *player : players) {
        if (distance(center->getPosition(), player->getPosition()) <= INTEREST_RADIUS) {
            visiblePlayers.push_back(player);
        }
    }
    return visiblePlayers;
}

std::vector<Projectile *> InterestSystem::getVisibleProjectiles(Player *center, const std::vector<Projectile *> projectiles) {
    std::vector<Projectile *> visibleProjectiles;
    for (Projectile *projectile : projectiles) {
        if (distance(center->getPosition(), projectile->getPosition()) <= INTEREST_RADIUS
            || projectile->getOwnerId() == center->getId()) {
            visibleProjectiles.push_back(projectile);
        }
    }
    return visibleProjectiles;
}