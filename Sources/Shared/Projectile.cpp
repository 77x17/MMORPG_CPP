#include "Projectile.hpp"

#include "Utils.hpp"

Projectile::Projectile(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction) 
    : Entity(-1, sf::Vector2f(10.0f, 10.0f)), 
      ownerId(_ownerId), lifeTimer(0), damage(10) 
{
    position = startPosition;
    velocity = normalize(direction) * _BULLET_SPEED;
}

void Projectile::update(const float &dt) {
    position  += velocity * dt;
    lifeTimer += dt;

    if (lifeTimer > BULLET_LIFETIME) destroy();
}

int Projectile::getOwnerId() const {
    return ownerId;
}

int Projectile::getDamage() const {
    return damage;
}

sf::Vector2f Projectile::getVelocity() const {
    return velocity;
}

void Projectile::setId(int _id) {
    id = _id;
}