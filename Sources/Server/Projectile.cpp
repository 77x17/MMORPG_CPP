#include "Projectile.hpp"

#include "../Shared/Constants.hpp"
#include "../Shared/Utils.hpp"

Projectile::Projectile(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction) 
: DamageEntity(_ownerId, startPosition, { 10.0f, 10.0f }) {
    velocity          = normalize(direction) * BULLET_SPEED;
    damage            = 10;
    knockbackStrength = 10.0f;
    lifeTimer         = BULLET_LIFETIME;
}

void Projectile::update(const float &dt) {
    position  += velocity * dt;
    
    lifeTimer -= dt;

    if (lifeTimer <= 0) destroy();
}

bool Projectile::canHitMultiple() const {
    return false;
}

sf::Vector2f Projectile::getVelocity() const {
    return velocity;
}