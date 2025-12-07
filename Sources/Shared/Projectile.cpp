#include "Projectile.hpp"

#include "Utils.hpp"

Projectile::Projectile(int _ownerId, const sf::Vector2f &position, const sf::Vector2f &direction) 
    : Entity(-1, sf::Vector2f(10.0f, 10.0f), sf::Color::Yellow), 
      ownerId(_ownerId), lifeTimer(0), damage(10) 
{
    setPosition(position);
    velocity = normalize(direction) * BULLET_SPEED;
}

void Projectile::update(const float &dt) {
    shape.move(velocity * dt);
    lifeTimer += dt;

    if (lifeTimer > BULLET_LIFETIME) destroy();
}

int Projectile::getOwnerId() const {
    return ownerId;
}

int Projectile::getDamage() const {
    return damage;
}