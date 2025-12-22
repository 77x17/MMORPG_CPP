#include "Server/Entities/Enemy.hpp"

#include "Shared/Constants.hpp"
#include "Shared/Utils.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

Enemy::Enemy(int id, const sf::Vector2f &startPosition) 
: Entity(id, sf::Vector2f( ENEMY_WIDTH, ENEMY_HEIGHT )),
    health(100), maxHealth(100),
    spawnPosition(startPosition),
    detectRange(250.0f)
{
    setPosition(startPosition);
}

void Enemy::update(const float &dt) {
    if (isDestroyed()) {
        respawn();
    }

    if (projectileCooldownTimer >= 0) {
        projectileCooldownTimer -= dt;
    }

    decayImpulse(dt);

    if (randomCooldownTimer >= 0) {
        randomCooldownTimer -= dt;
    }
}

void Enemy::respawn() {
    destroyed = false;
    health    = maxHealth;
    setPosition(spawnPosition);
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        destroy();

        LogSystem::addMessage("[Enemy] ID: " + std::to_string(id) + " died!");
    }
}

void Enemy::applyImpulse(const sf::Vector2f &_impulse) {
    impulse += _impulse;
}

void Enemy::decayImpulse(const float &dt) {
    constexpr float DECAY = 15.0f;
    impulse *= std::exp(-DECAY * dt);

    if (fabs(impulse.x) < 0.1f && fabs(impulse.y) < 0.1f) {
        impulse = { 0.0f, 0.0f };
    }
}

void Enemy::setOldShootDir(const sf::Vector2f &newPosition) {
    oldShootDir = newPosition;
}

void Enemy::setVelocity(const sf::Vector2f &newVelocity) {
    velocity = newVelocity;
}

void Enemy::resetProjectileCooldownTimer() {
    projectileCooldownTimer = PROJECTILE_COOLDOWN_TIME;
}

void Enemy::setPreviousMovement(const sf::Vector2f &newMovement) {
    previousMovement = newMovement;
}

void Enemy::resetRandomCooldownTimer() {
    randomCooldownTimer = RANDOM_COOLDOWN_TIME;
}

int Enemy::getHealth() const {
    return health;
}

const sf::Vector2f & Enemy::getOldShootDir() const {
    return oldShootDir;
}

const sf::Vector2f & Enemy::getImpulse() const {
    return impulse;
}

const float & Enemy::getProjectileCooldownTimer() const {
    return projectileCooldownTimer;
}

const float & Enemy::getDetectRange() const {
    return detectRange;
}

const sf::Vector2f & Enemy::getPreviousMovement() const {
    return previousMovement;
}

const float & Enemy::getRandomCooldownTimer() const {
    return randomCooldownTimer;
}

const sf::Vector2f & Enemy::getSpawnPosition() const {
    return spawnPosition;
}

AABB Enemy::buildAABB(const sf::Vector2f &atPosition) const {
    return { atPosition - size / 2.0f, size };
}