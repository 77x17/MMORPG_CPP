#pragma once

#include "Server/Entities/Entity.hpp"

enum class EnemyState {
    Idle,
    Chase,
    Attack,
    Dead
};

class Enemy : public Entity {
private:
    EnemyState state = EnemyState::Idle;

    int hp;
    float moveSpeed = 100.0f;

    int targetPlayerId = -1;
    float thinkTimer = 0.0f;

public:
    Enemy(int id, const sf::Vector2f &startPosition);

    void update(const float &dt) override;

    void takeDamage(int damage);

    EnemyState getState() const { return state; }
    int getHp() const { return hp; }
};