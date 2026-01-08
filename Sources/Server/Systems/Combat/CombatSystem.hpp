#pragma once

#include <vector>

#include "Server/Systems/Combat/KilledEvent.hpp"

class Enemy;
class Player;
class DamageEntity;

class GameWorld;

class CombatSystem {
private:
    std::vector<KilledEvent> killedEvents;

private:
    void handleCollision(const std::vector<Player *> &players, const std::vector<DamageEntity *> &damageEntities);
    void handleCollision(const std::vector<Enemy *>  &enemies, const std::vector<DamageEntity *> &damageEntities);

public:
    void handleCollision(const std::vector<Player *> &players, const std::vector<Enemy *> &enemies, const std::vector<DamageEntity *> &damageEntities);

    void handleKilledEvents(GameWorld &gameWorld);
};