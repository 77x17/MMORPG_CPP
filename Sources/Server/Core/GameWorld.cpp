#include "Server/Core/GameWorld.hpp"

#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp" 

#include "Server/Systems/Inventory/InventoryManager.hpp"

GameWorld::GameWorld() {}

void GameWorld::addPlayer(int id) {
    Player *newPlayer = new Player(id, sf::Vector2f(500, 500));

    if (not InventoryManager::loadInventory(newPlayer->getId(), newPlayer->getInventory())) {
        InventoryManager::saveInventory(newPlayer->getId(), newPlayer->getInventory());
    }
    if (not InventoryManager::loadEquipment(newPlayer->getId(), newPlayer->getEquipment())) {
        InventoryManager::saveEquipment(newPlayer->getId(), newPlayer->getEquipment());
    }
    
    players.push_back(newPlayer);
}

void GameWorld::removePlayer(int id) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i]->getId() == id) {
            InventoryManager::saveInventory(players[i]->getId(), players[i]->getInventory());
            InventoryManager::saveEquipment(players[i]->getId(), players[i]->getEquipment());

            delete players[i];
            players.erase(players.begin() + i);

            break;
        }
    }
}

void GameWorld::addDamageEntity(DamageEntity *newDamageEntity) {
    damageEntities.push_back(newDamageEntity);
}

void GameWorld::update(const float &dt) {
    for (Player* player : players) {
        player->update(dt);
    }

    for (DamageEntity* damageEntity : damageEntities) {
        damageEntity->update(dt);
    }

    for (size_t i = 0; i < damageEntities.size(); ++i) {
        if (damageEntities[i]->isDestroyed()) {
            delete damageEntities[i];
            damageEntities.erase(damageEntities.begin() + i);
            --i;
        }
    }
}

const std::vector<Player *> GameWorld::getPlayers() const {
    return players;
}

const std::vector<DamageEntity *> GameWorld::getDamageEntities() const {
    return damageEntities;
}

Player * GameWorld::getPlayer(int clientId) {
    for (Player *player : players) {
        if (player->getId() == clientId) return player;
    }
    return nullptr;
}

bool GameWorld::moveItem(int clientId, int from, int to) {
    Player *player = getPlayer(clientId);
    if (player == nullptr) return false;
    
    if (player->getInventory()[from].id != -1 || player->getInventory()[to].id != -1) {
        std::swap(player->getInventory()[from].id, player->getInventory()[to].id);
        return true;
    }
    return false;
}

bool GameWorld::equipItem(int clientId, int fromInventory, int toEquipment) {
    Player *player = getPlayer(clientId);
    if (player == nullptr) return false;

    if (player->getInventory()[fromInventory].id != -1 || player->getEquipment()[toEquipment].id != -1) {
        std::swap(player->getInventory()[fromInventory].id, player->getEquipment()[toEquipment].id);
        return true;
    }
    return false;
}