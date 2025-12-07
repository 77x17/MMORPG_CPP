#pragma once

const float PLAYER_SPEED   = 200.0f;
const float COOLDOWN_TIMER = 1.0f;

#include "Entity.hpp"
#include "InputState.hpp"

class Projectile;

class Player : public Entity {
private:
    int health;
    int maxHealth;
    float cooldownTimer;
    sf::Vector2f oldShootDir = {1.0f, 0.0f};
    
public:
    int lastProcessedInput = 0;

    Player(int _id, const sf::Vector2f &startPosition, const sf::Color &color);

    Projectile* updatePlayer(const float &dt, const InputState &input);

    void update(const float &dt) override;

    void takeDamage(int amount);

    int getHealth() const;
};