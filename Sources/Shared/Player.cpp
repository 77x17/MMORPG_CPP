#include "Player.hpp"

#include <iostream>

#include "Utils.hpp"
#include "Projectile.hpp"

Player::Player(int _id, const sf::Vector2f &startPosition) 
    : Entity(_id, sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT)), 
      health(100), maxHealth(100), projectileCooldownTimer(0) 
{
    position = startPosition;
}

Projectile* Player::updatePlayer(const float &dt, const InputState &input) {
    if (isDestroyed()) {
        health    = maxHealth;
        position  = sf::Vector2f(100.0f, 100.0f);
        destroyed = false;
    }

    // Check movement
    if (input.movementDir.x != 0 || input.movementDir.y != 0) {
        sf::Vector2f velocity = input.movementDir * _PLAYER_SPEED;
        position += velocity * dt;
        // shape.move(velocity * dt);
        oldShootDir = input.movementDir;
    }

    // Keep player inside the playground
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x > WINDOW_WIDTH ) position.x = WINDOW_WIDTH;
    if (position.y > WINDOW_HEIGHT) position.y = WINDOW_HEIGHT;

    // Check shooting
    if (projectileCooldownTimer > 0) projectileCooldownTimer -= dt;

    if (input.isShooting && projectileCooldownTimer <= 0) {
        projectileCooldownTimer = PROJECTILE_COOLDOWN_TIMER;
        sf::Vector2f shootDir = input.movementDir;
        if (shootDir.x == 0 && shootDir.y == 0) shootDir = oldShootDir;
        return new Projectile(id, position, shootDir);
    }

    return nullptr;
}

void Player::update(const float &dt) {}

void Player::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        destroy();
        std::cout << "Player " << id << " DIED!" << '\n';
    }
    std::cout << "Player " << id << " took " << amount << " dmg. HP: " << health << '\n';
}

int Player::getHealth() const {
    return health;
}