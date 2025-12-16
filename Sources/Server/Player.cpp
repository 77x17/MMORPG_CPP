#include "Player.hpp"

#include <iostream>

#include "../Shared/Constants.hpp"
#include "../Shared/Utils.hpp"

Player::Player(int _id, const sf::Vector2f &startPosition) 
    : Entity(_id, sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT)), 
      health(100), maxHealth(100), 
      spawnPosition(startPosition)
{
    position = spawnPosition;
    velocity = impulse = { 0.0f, 0.0f };
}

void Player::update(const float &dt) {
    if (isDestroyed()) {
        health    = maxHealth;
        position  = spawnPosition;
        destroyed = false;
    }

    if (projectileCooldownTimer > 0) projectileCooldownTimer -= dt;

    constexpr float DECAY = 15.0f;
    impulse *= std::exp(-DECAY * dt);

    if (fabs(impulse.x - impulse.y) < 5.0f) {
        impulse = {0.f, 0.f};
    }
}

void Player::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        destroy();
        // std::cout << "Player " << id << " DIED!" << '\n';
    }
    // std::cout << "Player " << id << " took " << amount << " dmg. HP: " << health << '\n';
}

void Player::applyImpulse(const sf::Vector2f &_impulse) {
    impulse += _impulse;
}

int Player::getHealth() const {
    return health;
}

const sf::Vector2f & Player::getOldShootDir() const {
    return oldShootDir;
}

const sf::Vector2f & Player::getImpulse() const {
    return impulse;
}

Inventory & Player::getInventory() {
    return inventory;
}

Equipment & Player::getEquipment() {
    return equipment;
}

AABB Player::buildAABB(const sf::Vector2f &atPosition) const {
    return { atPosition - size / 2.0f, size };
}

void Player::setOldShootDir(const sf::Vector2f &newPosition) {
    oldShootDir = newPosition;
}

void Player::setVelocity(const sf::Vector2f &newVelocity) {
    velocity = newVelocity;
}