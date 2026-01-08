#pragma once

#include "Shared/AABB.hpp"

#include "Server/Entities/Entity.hpp"
#include "Server/Systems/Inventory/Inventory.hpp"
#include "Server/Systems/Inventory/Equipment.hpp"

#include "Server/Quests/PlayerQuestState.hpp"

class Player : public Entity {
private:
    int          clientId;

    int          health;
    int          maxHealth;
    
    sf::Vector2f spawnPosition;
    sf::Vector2f oldShootDir = { 1.0f, 0.0f };
    sf::Vector2f velocity;
    sf::Vector2f impulse;
    
    sf::Vector2f mousePosition;
    bool         mouseSelected    = false;
    int          entitySelectedId = -1;

    Inventory inventory;
    Equipment equipment;
    
    float projectileCooldownTimer = 0;
    const float PROJECTILE_COOLDOWN_TIME = 0.1f;

    float respawnCooldownTimer = 0.0f;
    const float RESPAWN_COOLDOWN_TIME = 5.0f;
    
    std::vector<PlayerQuestState> quests;

public:
    int lastProcessedInput = 0;

    Player(int _entityId, int _clientId, const sf::Vector2f &startPosition);

    void update(const float &dt) override;

    void respawn();
    void takeDamage(int damage);
    void applyImpulse(const sf::Vector2f &_impulse);
    void decayImpulse(const float &dt);
    
    void setOldShootDir(const sf::Vector2f &newPosition);
    void setVelocity(const sf::Vector2f &newVelocity);
    void resetProjectileCooldownTimer();

    // === Mouse selected ===
    void setMousePosition(const sf::Vector2f &newPosition);
    void setEntitySelectedId(int newId);
    void toggleMouseSelected();
    int getEntitySelectedId() const;
    const sf::Vector2f & getMousePosition() const;
    bool getMouseSelected() const;

    int getHealth() const;
    const sf::Vector2f & getOldShootDir() const;
    const sf::Vector2f & getImpulse() const;
    const float & getProjectileCooldownTimer() const;
    
    Inventory & getInventory();
    Equipment & getEquipment();

    AABB buildAABB(const sf::Vector2f &atPosition) const;
    
    int getClientId() const;
    void setClientId(int _clientId);

    // === Quests ===
    std::vector<PlayerQuestState> & getQuests();
};