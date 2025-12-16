#pragma once

#include "../Shared/AABB.hpp"

#include "Entity.hpp"
#include "Inventory.hpp"
#include "Equipment.hpp"

class Player : public Entity {
private:
    int          health;
    int          maxHealth;
    
    sf::Vector2f spawnPosition;
    sf::Vector2f oldShootDir = { 1.0f, 0.0f };
    sf::Vector2f velocity;
    float knockback_strength;
    
    Inventory inventory;
    Equipment equipment;
    
public:
    int lastProcessedInput = 0;
    float projectileCooldownTimer = 0;

    Player(int _id, const sf::Vector2f &startPosition);

    void update(const float &dt) override;

    void takeDamage(int amount);
    void knockback(const sf::Vector2f &direction, const float &knockback);
    void move(const sf::Vector2f &direction);

    void setOldShootDir(const sf::Vector2f &newPosition);
    void setVelocity(const sf::Vector2f &newVelocity);

    int getHealth() const;
    sf::Vector2f getVelocity() const;
    float getKnockback() const;
    sf::Vector2f getOldShootDir() const;

    Inventory & getInventory();
    Equipment & getEquipment();

    AABB buildAABB(const sf::Vector2f &atPosition) const;
};