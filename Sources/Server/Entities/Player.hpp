#pragma once

#include "Shared/AABB.hpp"

#include "Server/Entities/Entity.hpp"
#include "Server/Systems/Inventory/Inventory.hpp"
#include "Server/Systems/Inventory/Equipment.hpp"

class Player : public Entity {
private:
    int          health;
    int          maxHealth;
    
    sf::Vector2f spawnPosition;
    sf::Vector2f oldShootDir = { 1.0f, 0.0f };
    sf::Vector2f velocity;
    sf::Vector2f impulse;
    
    Inventory inventory;
    Equipment equipment;
    
    float projectileCooldownTimer = 0;
    const float PROJECTILE_COOLDOWN_TIME = 0.1f;

public:
    int lastProcessedInput = 0;

    Player(int _id, const sf::Vector2f &startPosition);

    void update(const float &dt) override;

    void respawn();
    void takeDamage(int amount);
    void applyImpulse(const sf::Vector2f &_impulse);
    void decayImpulse(const float &dt);
    
    void setOldShootDir(const sf::Vector2f &newPosition);
    void setVelocity(const sf::Vector2f &newVelocity);
    void resetProjectileCooldownTimer();

    int getHealth() const;
    const sf::Vector2f & getOldShootDir() const;
    const sf::Vector2f & getImpulse() const;
    const float & getProjectileCooldownTimer() const;

    Inventory & getInventory();
    Equipment & getEquipment();

    AABB buildAABB(const sf::Vector2f &atPosition) const;
};