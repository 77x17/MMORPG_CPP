#include "SwordSlash.hpp"

#include "../Shared/Utils.hpp"

SwordSlash::SwordSlash(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &_direction) 
: DamageEntity(_ownerId, startPosition, { 40.0f, 40.0f }) {
    direction         = normalize(_direction);
    range             = 30.0f;
    damage            = 10.0f;
    knockbackStrength = 10.0f;
    lifeTimer         = 0.15f;
}

void SwordSlash::update(const float &dt) {
    lifeTimer -= dt;
    if (lifeTimer <= 0) {
        destroy();
    }
}

bool SwordSlash::canHitMultiple() const {
    return true;
}

sf::FloatRect SwordSlash::getBound() const {
    sf::Vector2f hitPosition = position + direction * range;
    return { hitPosition, size };
}