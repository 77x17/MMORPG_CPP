#include "Player.hpp"

#include "Utils.hpp"
#include "Projectile.hpp"

Player::Player(int _id, const sf::Vector2f &startPosition, const sf::Color &color) 
    : Entity(_id, sf::Vector2f(40.0f, 40.0f), color), 
      health(100), maxHealth(100), cooldownTimer(0) 
{
    setPosition(startPosition);
}

Projectile* Player::updatePlayer(const float &dt, const InputState &input) {
    // Check movement
    if (input.movementDir.x != 0 || input.movementDir.y != 0) {
        sf::Vector2f velocity = input.movementDir * PLAYER_SPEED;
        shape.move(velocity * dt);
        oldShootDir = input.movementDir;
    }

    // Keep player inside the playground
    sf::Vector2f position = getPosition();
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x > WINDOW_WIDTH ) position.x = WINDOW_WIDTH;
    if (position.y > WINDOW_HEIGHT) position.y = WINDOW_HEIGHT;
    setPosition(position);

    // Check shooting
    if (cooldownTimer > 0) cooldownTimer -= dt;

    if (input.isShooting && cooldownTimer <= 0) {
        cooldownTimer = COOLDOWN_TIMER;
        sf::Vector2f shootDir = input.movementDir;
        if (shootDir.x == 0 && shootDir.y == 0) shootDir = oldShootDir;
        return new Projectile(id, getPosition(), shootDir);
    }

    return nullptr;
}

void Player::update(const float &dt) {

}

void Player::takeDamage(int amount) {
    health -= amount;
    std::cout << "Player " << id << " took " << amount << " dmg. HP: " << health << '\n';
    if (health <= 0) {
        health = 0;
        destroy();
        std::cout << "Player " << id << " DIED!" << '\n';
    }
}

int Player::getHealth() const {
    return health;
}