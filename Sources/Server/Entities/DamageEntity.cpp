#include "Server/Entities/DamageEntity.hpp"

DamageEntity::DamageEntity(int _ownerId, const sf::Vector2f &_position, const sf::Vector2f &_size) 
: Entity(-1, _size), ownerId(_ownerId) {
    position = _position;
}

int DamageEntity::getOwnerId() const {
    return ownerId;
}

int DamageEntity::getDamage() const {
    return damage;
}

float DamageEntity::getKnockbackStrength() const {
    return knockbackStrength;
}