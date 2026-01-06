#pragma once

#include "Server/Entities/Entity.hpp"

class Enemy;
class Player;

class DamageEntity : public Entity {
protected:
    int   ownerEntityId;
    int   damage;
    float knockbackStrength;
    float lifeTimer;
    float displayTimer = 0.0f;

public:
    DamageEntity(int _ownerEntityId, const sf::Vector2f &_position, const sf::Vector2f &_size);
    virtual ~DamageEntity() = default;

    int getOwnerEntityId() const;
    int getDamage() const;
    float getKnockbackStrength() const;

    virtual bool canHitMultiple() const = 0;
    
    virtual void onHit(Player &player) = 0;
    virtual void onHit(Enemy  &enemy) = 0;
    virtual std::string getName() const = 0;
};