#pragma once

#include "Server/Entities/DamageEntity.hpp"

class Projectile : public DamageEntity {
private:
    sf::Vector2f velocity;
    
public:
    Projectile(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction);

    void update(const float &dt) override;

    bool canHitMultiple() const override;

    sf::Vector2f getVelocity() const;

    void onHit(Player &player) override;
    void onHit(Enemy  &enemy ) override;
};