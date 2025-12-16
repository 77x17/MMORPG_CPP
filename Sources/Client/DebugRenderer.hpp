#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

class EntityManager;
class WorldCollision;

class DebugRenderer {
private:
    sf::RenderWindow &window;
    bool enabled = true;

private:
    void drawWorldCollision(WorldCollision &worldCollision);

public:
    DebugRenderer(sf::RenderWindow &_window);

    void toggle();
    bool isEnabled() const;

    void render(const EntityManager &entityManager, WorldCollision &worldCollision, int clientId);
};