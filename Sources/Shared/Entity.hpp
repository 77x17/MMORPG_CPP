#pragma once

#include <SFML/Graphics.hpp>

class Entity {
protected:
    int id;
    sf::RectangleShape shape;
    bool destroyed = false;

public:
    Entity(int _id, const sf::Vector2f &size, const sf::Color &color);
    virtual ~Entity();

    virtual void update(const float &dt) = 0;

    void draw(sf::RenderWindow &window);

    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f &position);
    sf::FloatRect getBounds() const;
    int getId() const;

    bool isDestroyed() const;
    void destroy();
};