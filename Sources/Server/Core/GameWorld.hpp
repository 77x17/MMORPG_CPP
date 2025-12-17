#pragma once

#include <vector>

class Player;
class DamageEntity;

class GameWorld {
private:
    std::vector<Player *> players;
    
    int nextProjectileId;
    std::vector<DamageEntity *> damageEntities;

public:
    GameWorld();

    void addPlayer(int id);
    void removePlayer(int id);

    void addDamageEntity(DamageEntity *newDamageEntity);

    void update(const float &dt);

    const std::vector<Player *> getPlayers() const;
    const std::vector<DamageEntity *> getDamageEntities() const;

    Player * getPlayer(int clientId);

    bool moveItem(int clientId, int from, int to);
    bool equipItem(int clientId, int fromInventory, int toEquipment);
};