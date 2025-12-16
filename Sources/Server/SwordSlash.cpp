#include "SwordSlash.hpp"

#include "../Shared/Utils.hpp"

#include "Player.hpp"

SwordSlash::SwordSlash(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &_direction) 
: DamageEntity(_ownerId, startPosition, { 40.0f, 40.0f }) {
    direction         = normalize(_direction);
    range             = 30.0f;
    damage            = 20.0f;
    knockbackStrength = 20.0f;
    lifeTimer         = 0.15f;
    displayTimer      = 0.15f;

    setPosition(position + direction * range);
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

    sf::Vector2f direction = position - player.getPosition();
    player.knockback(direction, knockbackStrength);
}