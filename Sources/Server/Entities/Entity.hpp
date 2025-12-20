#pragma once

#include <SFML/Graphics.hpp>

class Entity {
protected:
    int          id;
    sf::Vector2f position = { 0.0f, 0.0f };
    sf::Vector2f oldPosition;
    sf::Vector2f size;
    bool         destroyed = false;

public:
    Entity(int newId, const sf::Vector2f &newSize);
    virtual ~Entity();

    virtual void update(const float &dt) = 0;

    void draw(sf::RenderWindow &window);

    sf::Vector2f getPosition() const;
    sf::Vector2f getOldPosition() const;
    void setPosition(const sf::Vector2f &newPosition);
    void setOldPosition(const sf::Vector2f &newPosition);
    sf::FloatRect getBounds() const;
    int getId() const;
    void setId(int _id);

    virtual bool isDestroyed() const;
    virtual void destroy();
};