#pragma once

#include "DamageEntity.hpp"
#include "../Shared/InputState.hpp"

class Projectile;

class Player : public Entity {
private:
    int          health;
    int          maxHealth;
    float        projectileCooldownTimer;
    sf::Vector2f oldShootDir = { 1.0f, 0.0f };
    sf::Vector2f velocity;
    float knockback_strength;

public:
    int lastProcessedInput = 0;

    Player(int _id, const sf::Vector2f &startPosition);

    DamageEntity* updatePlayer(const float &dt, const InputState &input);

    void update(const float &dt) override;

    void takeDamage(int amount);
    void knockback(const sf::Vector2f &direction, const float &knockback);

    int getHealth() const;
    sf::Vector2f getVelocity() const;
    float getKnockback() const;
};