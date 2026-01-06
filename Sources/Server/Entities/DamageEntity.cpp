#include "Server/Entities/DamageEntity.hpp"

DamageEntity::DamageEntity(int _ownerEntityId, const sf::Vector2f &_position, const sf::Vector2f &_size) 
: Entity(-1, _size), ownerEntityId(_ownerEntityId) {
    position = _position;
}

int DamageEntity::getOwnerEntityId() const {
    return ownerEntityId;
}

int DamageEntity::getDamage() const {
    return damage;
}

float DamageEntity::getKnockbackStrength() const {
    return knockbackStrength;
}