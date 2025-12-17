#include "Server/Entities/Enemy.hpp"

Enemy::Enemy(int id, const sf::Vector2f &startPosition) 
: Entity(id, sf::Vector2f( 40.0f, 40.0f )) {
    setPosition(startPosition);
}

void Enemy::update(const float &dt) {
}

void Enemy::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        destroy();
        state = EnemyState::Dead;
    }
}
