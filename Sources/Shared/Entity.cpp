#include "Entity.hpp"

Entity::Entity(int _id, const sf::Vector2f &size, const sf::Color &color) : id(_id) {
    shape.setSize(size);
    shape.setFillColor(color);
    shape.setOrigin(size.x / 2, size.y / 2);
}

Entity::~Entity() {

}

void Entity::draw(sf::RenderWindow &window) {
    window.draw(shape);
}

sf::Vector2f Entity::getPosition() const {
    return shape.getPosition();
}

void Entity::setPosition(const sf::Vector2f &position) {
    shape.setPosition(position);
} 

sf::FloatRect Entity::getBounds() const {
    return shape.getGlobalBounds();
}

int Entity::getId() const {
    return id;
}

bool Entity::isDestroyed() const {
    return destroyed;
}

void Entity::destroy() {
    destroyed = true;
}
