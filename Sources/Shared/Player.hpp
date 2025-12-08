#pragma once

const float _PLAYER_SPEED             = 200.0f;
const float PROJECTILE_COOLDOWN_TIMER = 0.1f;
const float PLAYER_WIDTH              = 40.0f;
const float PLAYER_HEIGHT             = 40.0f;

#include "Entity.hpp"
#include "InputState.hpp"

class Projectile;

class Player : public Entity {
private:
    int          health;
    int          maxHealth;
    float        projectileCooldownTimer;
    sf::Vector2f oldShootDir = { 1.0f, 0.0f };
    
public:
    int lastProcessedInput = 0;

    Player(int _id, const sf::Vector2f &startPosition);

    Projectile* updatePlayer(const float &dt, const InputState &input);

    void update(const float &dt) override;

    void takeDamage(int amount);

    int getHealth() const;
};