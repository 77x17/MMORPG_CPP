#pragma once

#include <SFML/System/Vector2.hpp>

struct EntityState {
    int  id        = -1;
    sf::Vector2f position{ 0.0f, 0.0f };
    sf::Vector2f velocity{ 0.0f, 0.0f };
    int  hp        = 0;
    bool destroyed = false;
};

class IEntity {
public:
    virtual ~IEntity() = default;

    virtual int getId() const = 0;
    virtual sf::Vector2f getPosition() const = 0;
    virtual void setPosition(const sf::Vector2f &newPosition) = 0;
    virtual bool isDestroyed() const = 0;

    virtual EntityState toState() const = 0;
};