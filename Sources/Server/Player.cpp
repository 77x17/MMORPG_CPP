#include "Player.hpp"

#include <iostream>

#include "../Shared/Constants.hpp"
#include "../Shared/Utils.hpp"

Player::Player(int _id, const sf::Vector2f &startPosition) 
    : Entity(_id, sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT)), 
      health(100), maxHealth(100), 
      spawnPosition(startPosition)
{
    position           = spawnPosition;
    knockback_strength = 10.0f;
}

void Player::update(const float &dt) {
    if (isDestroyed()) {
        health    = maxHealth;
        position  = spawnPosition;
        destroyed = false;
    }

    if (projectileCooldownTimer > 0) projectileCooldownTimer -= dt;
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

void Player::knockback(const sf::Vector2f &direction, const float &knockback) {
    position -= normalize(direction) * knockback;
}

void Player::move(const sf::Vector2f &direction) {
    position += direction;
}

int Player::getHealth() const {
    return health;
}

sf::Vector2f Player::getVelocity() const {
    return velocity;
}

float Player::getKnockback() const {
    return knockback_strength;
}

Inventory & Player::getInventory() {
    return inventory;
}

Equipment & Player::getEquipment() {
    return equipment;
}

AABB Player::buildAABB(const sf::Vector2f &atPosition) const {
    return { atPosition, size };
}

void Player::setOldShootDir(const sf::Vector2f &newPosition) {
    oldShootDir = newPosition;
}

void Player::setVelocity(const sf::Vector2f &newVelocity) {
    velocity = newVelocity;
}

sf::Vector2f Player::getOldShootDir() const {
    return oldShootDir;
}