#include "Server/Entities/DamageEntity.hpp"

DamageEntity::DamageEntity(int _ownerEntityId, EntityType _ownerType, const sf::Vector2f &_position, const sf::Vector2f &_size) 
: Entity(-1, _size), ownerEntityId(_ownerEntityId), ownerType(_ownerType) {
    position = _position;
}

int DamageEntity::getOwnerEntityId() const {
    return ownerEntityId;
}

EntityType DamageEntity::getOwnerType() const {
    return ownerType;
}

int DamageEntity::getDamage() const {
    return damage;
}

float DamageEntity::getKnockbackStrength() const {
    return knockbackStrength;
}