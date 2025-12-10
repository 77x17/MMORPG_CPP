#pragma once

#include "DamageEntity.hpp"

class Projectile : public DamageEntity {
private:
    sf::Vector2f velocity;
    
public:
    Projectile(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction);

    void update(const float &dt) override;

    bool canHitMultiple() const override;

    void setId(int id);

    sf::Vector2f getVelocity() const;
};