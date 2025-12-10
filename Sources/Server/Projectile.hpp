#pragma once

#include "Entity.hpp"

class Projectile : public Entity {
private:
    sf::Vector2f velocity;
    int ownerId;
    float lifeTimer;
    int damage;
    float knockback_strength;

public:
    Projectile(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction);

    void update(const float &dt) override;

    void setId(int _id);

    int getOwnerId() const;

    int getDamage() const;

    sf::Vector2f getVelocity() const;

    float getKnockback() const;
};