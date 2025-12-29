#pragma once

#include "Shared/AABB.hpp"

#include "Server/Entities/Entity.hpp"

class Enemy : public Entity {
private:
    int health;
    int maxHealth;

    sf::Vector2f spawnPosition;
    sf::Vector2f oldShootDir = { 1.0f, 0.0f };
    sf::Vector2f velocity;
    sf::Vector2f impulse;
    sf::Vector2f previousMovement;

    float projectileCooldownTimer = 0;
    const float PROJECTILE_COOLDOWN_TIME = 1.0f;

    float detectRange;
    float randomCooldownTimer = 0.0f;
    const float RANDOM_COOLDOWN_TIME = 3.0f;

public:
    int inputSeq = 0;
    int lastProcessedInput = 0;
    
    Enemy(int id, const sf::Vector2f &startPosition);

    void update(const float &dt) override;

    void respawn();
    void takeDamage(int damage);
    void applyImpulse(const sf::Vector2f &_impulse);
    void decayImpulse(const float &dt);

    void setOldShootDir(const sf::Vector2f &newPosition);
    void setVelocity(const sf::Vector2f &newVelocity);
    void resetProjectileCooldownTimer();
    void setPreviousMovement(const sf::Vector2f &newMovement);
    void resetRandomCooldownTimer();

    int getHealth() const;
    int getMaxHealth() const;
    
    const sf::Vector2f & getOldShootDir() const;
    const sf::Vector2f & getImpulse() const;
    const float & getProjectileCooldownTimer() const;
    const float & getDetectRange() const;
    const sf::Vector2f & getPreviousMovement() const;
    const float & getRandomCooldownTimer() const;
    const sf::Vector2f & getSpawnPosition() const;

    AABB buildAABB(const sf::Vector2f &atPosition) const;
};