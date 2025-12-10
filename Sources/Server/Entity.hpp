#pragma once

#include <SFML/Graphics.hpp>

class Entity {
protected:
    int          id;
    sf::Vector2f position;
    sf::Vector2f size;
    bool         destroyed = false;

public:
    Entity(int newId, const sf::Vector2f &newSize);
    virtual ~Entity();

    virtual void update(const float &dt) = 0;

    void draw(sf::RenderWindow &window);

    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f &newPosition);
    sf::FloatRect getBounds() const;
    int getId() const;
    void setId(int _id);

    bool isDestroyed() const;
    void destroy();
};