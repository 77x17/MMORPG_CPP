#include "Server/Entities/Player.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

#include "Shared/Constants.hpp"
#include "Shared/Utils.hpp"

Player::Player(int _entityId, int _clientId, const sf::Vector2f &startPosition) 
    : Entity(_entityId, sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT)), clientId(_clientId), 
      health(100), maxHealth(100), 
      spawnPosition(startPosition)
{
    oldPosition = spawnPosition;
    position = spawnPosition;
    velocity = impulse = { 0.0f, 0.0f };
}

void Player::update(const float &dt) {
    if (projectileCooldownTimer > 0) projectileCooldownTimer -= dt;
    
    if (isDestroyed()) {
        respawnCooldownTimer += dt;

        if (respawnCooldownTimer >= RESPAWN_COOLDOWN_TIME) {
            respawnCooldownTimer = 0.0f;

            respawn();
        }
    }

    decayImpulse(dt);
}

void Player::respawn() {
    destroyed = false;
    health    = maxHealth;
    setPosition(spawnPosition);
}

void Player::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        destroy();
        LogSystem::addMessage("[Player] ID: " + std::to_string(clientId) + " died!");
    }
}

void Player::applyImpulse(const sf::Vector2f &_impulse) {
    impulse += _impulse;
}

void Player::decayImpulse(const float &dt) {
    constexpr float DECAY = 15.0f;
    impulse *= std::exp(-DECAY * dt);

    if (fabs(impulse.x) < 0.1f && fabs(impulse.y) < 0.1f) {
        impulse = { 0.0f, 0.0f };
    }
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

const float & Player::getProjectileCooldownTimer() const {
    return projectileCooldownTimer;
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

void Player::resetProjectileCooldownTimer() {
    projectileCooldownTimer = PROJECTILE_COOLDOWN_TIME;
}

void Player::setMousePosition(const sf::Vector2f &newPosition) {
    mousePosition = newPosition;
}

void Player::setEntitySelectedId(int newId) {
    entitySelectedId = newId;
}

int Player::getEntitySelectedId() const {
    return entitySelectedId;
}

const sf::Vector2f & Player::getMousePosition() const {
    return mousePosition;
}

void Player::toggleMouseSelected() {
    mouseSelected = !mouseSelected;
}

bool Player::getMouseSelected() const {
    return mouseSelected;
}

int Player::getClientId() const {
    return clientId;
}

void Player::setClientId(int _clientId) {
    clientId = _clientId;
}