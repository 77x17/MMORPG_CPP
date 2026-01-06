#include "Server/Entities/Entity.hpp"

Entity::Entity(int newId, const sf::Vector2f &newSize) 
: entityId(newId), size(newSize) {

}

Entity::~Entity() {

}

void Entity::draw(sf::RenderWindow &window) {

}

sf::Vector2f Entity::getPosition() const {
    return position;
}

sf::Vector2f Entity::getOldPosition() const {
    return oldPosition;
}

void Entity::setPosition(const sf::Vector2f &newPosition) {
    oldPosition = position;
    position    = newPosition;
} 

void Entity::setOldPosition(const sf::Vector2f &newPosition) {
    oldPosition = newPosition;
}

sf::FloatRect Entity::getBounds() const {
    return sf::FloatRect(position - size / 2.0f, size);
}

int Entity::getEntityId() const {
    return entityId;
}


void Entity::setEntityId(int _id) {
    entityId = _id;
}

bool Entity::isDestroyed() const {
    return destroyed;
}

void Entity::destroy() {
    destroyed = true;
}
