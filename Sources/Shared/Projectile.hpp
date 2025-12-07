#pragma once

const float BULLET_SPEED    = 400.0f;
const float BULLET_LIFETIME = 2.0f;

#include "Entity.hpp"

class Projectile : public Entity {
private:
    sf::Vector2f velocity;
    int ownerId;
    float lifeTimer;
    int damage;

public:
    Projectile(int _ownerId, const sf::Vector2f &position, const sf::Vector2f &direction);

    void update(const float &dt) override;

    void setId(int _id);

    int getOwnerId() const;

    int getDamage() const;

    sf::Vector2f getVelocity() const;
};