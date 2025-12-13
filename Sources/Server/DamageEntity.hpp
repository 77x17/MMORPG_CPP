#pragma once

#include "Entity.hpp"

class DamageEntity : public Entity {
protected:
    int   ownerId;
    int   damage;
    float knockbackStrength;
    float lifeTimer;
    float displayTimer = 0.0f;

public:
    DamageEntity(int _ownerId, const sf::Vector2f &_position, const sf::Vector2f &_size);

    virtual bool canHitMultiple() const = 0;

    int getOwnerId() const;
    virtual int getDamage() const;
    float getKnockbackStrength() const;
};