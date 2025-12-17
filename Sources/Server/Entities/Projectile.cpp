#include "Server/Entities/Projectile.hpp"

#include "Shared/Constants.hpp"
#include "Shared/Utils.hpp"

#include "Server/Entities/Player.hpp"

Projectile::Projectile(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction) 
: DamageEntity(_ownerId, startPosition, { 10.0f, 10.0f }) {
    velocity          = normalize(direction) * BULLET_SPEED;
    damage            = 5;
    knockbackStrength = 250.0f;
    lifeTimer         = BULLET_LIFETIME;
}

void Projectile::update(const float &dt) {
    oldPosition = position;
    position   += velocity * dt;
    
    lifeTimer -= dt;

    if (lifeTimer <= 0) destroy();
}

bool Projectile::canHitMultiple() const {
    return false;
}

sf::Vector2f Projectile::getVelocity() const {
    return velocity;
}

void Projectile::onHit(Player &player) {
    player.takeDamage(damage);
    sf::Vector2f direction = normalize(velocity);
    player.applyImpulse(direction * knockbackStrength);
}