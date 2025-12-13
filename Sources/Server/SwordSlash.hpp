#pragma once

#include "DamageEntity.hpp"

class SwordSlash : public DamageEntity {
private:
    sf::Vector2f direction;
    float        range;
    bool         hitEnemy = false;

public:
    SwordSlash(int _ownerId, const sf::Vector2f &startPosition, const sf::Vector2f &direction);

    void update(const float &dt) override;

    bool isDestroyed() const override;

    void destroy() override;

    bool canHitMultiple() const override;
};