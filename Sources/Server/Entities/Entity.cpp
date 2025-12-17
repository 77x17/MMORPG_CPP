#include "Server/Entities/Entity.hpp"

Entity::Entity(int newId, const sf::Vector2f &newSize) 
: id(newId), size(newSize) {

}

Entity::~Entity() {

}

void Entity::draw(sf::RenderWindow &window) {

}

sf::Vector2f Entity::getPosition() const {
    return position;
}

void Entity::setPosition(const sf::Vector2f &newPosition) {
    position = newPosition;
} 

sf::FloatRect Entity::getBounds() const {
    return sf::FloatRect(position - size / 2.0f, size);
}

int Entity::getId() const {
    return id;
}


void Entity::setId(int _id) {
    id = _id;
}

bool Entity::isDestroyed() const {
    return destroyed;
}

void Entity::destroy() {
    destroyed = true;
}
