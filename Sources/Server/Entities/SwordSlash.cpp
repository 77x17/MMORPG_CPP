#include "Server/Entities/SwordSlash.hpp"

#include "Shared/Utils.hpp"

#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"

SwordSlash::SwordSlash(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &_direction) 
: DamageEntity(_ownerId, startPosition, { 40.0f, 40.0f }) {
    direction         = normalize(_direction);
    range             = 30.0f;
    damage            = 10;
    knockbackStrength = 400.0f;
    lifeTimer         = 0.15f;
    displayTimer      = 0.15f;

    setPosition(position + direction * range);
    oldPosition = position;
}

void SwordSlash::update(const float &dt) {
    lifeTimer    -= dt;
    displayTimer -= dt;
    if (lifeTimer <= 0) {
        destroy();
    }
}

bool SwordSlash::canHitMultiple() const {
    return true;
}

bool SwordSlash::isDestroyed() const {
    return (displayTimer <= 0);
}

void SwordSlash::destroy() {
    damage = 0;
    knockbackStrength = 0;
}

void SwordSlash::onHit(Player &player) {
    player.takeDamage(damage);

    sf::Vector2f direction = normalize(player.getPosition() - position);
    player.applyImpulse(direction * knockbackStrength);
}

void SwordSlash::onHit(Enemy &enemy) {
    enemy.takeDamage(damage);

    sf::Vector2f direction = normalize(enemy.getPosition() - position);
    enemy.applyImpulse(direction * knockbackStrength);
}