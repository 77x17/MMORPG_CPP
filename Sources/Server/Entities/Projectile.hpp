#pragma once

#include "Server/Entities/DamageEntity.hpp"

class Projectile : public DamageEntity {
private:
    sf::Vector2f velocity;
    
public:
    Projectile(int _ownerId, EntityType _entityType, const sf::Vector2f &startPosition, const sf::Vector2f &direction, int _damage = 5);

    void update(const float &dt) override;

    bool canHitMultiple() const override;

    sf::Vector2f getVelocity() const;

    void onHit(Player &player) override;
    void onHit(Enemy  &enemy ) override;

    std::string getName() const override;
};