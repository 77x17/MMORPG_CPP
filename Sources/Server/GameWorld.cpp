#include "GameWorld.hpp"

void GameWorld::addPlayer(int id) {
    Player *newPlayer = new Player(id, sf::Vector2f(100, 100));
    players.push_back(newPlayer);
}

void GameWorld::removePlayer(int id) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i]->getId() == id) {
            delete players[i];
            players.erase(players.begin() + i);

            break;
        }
    }
}

void GameWorld::addProjectile(Projectile *newProjectile) {
    projectiles.push_back(newProjectile);
}

void GameWorld::update(const float &dt) {
    for (Player* player : players) {
        player->update(dt);
    }

    for (Projectile* projectile : projectiles) {
        projectile->update(dt);
    }

    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (projectiles[i]->isDestroyed()) {
            delete projectiles[i];
            projectiles.erase(projectiles.begin() + i);
            --i;
        }
    }
}

const std::vector<Player *> GameWorld::getPlayers() const {
    return players;
}

const std::vector<Projectile *> GameWorld::getProjectiles() const {
    return projectiles;
}