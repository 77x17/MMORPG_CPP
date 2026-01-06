#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace sf { class RenderWindow; }
namespace sf { class View; }

class EntityManager;

class MinimapRenderer {
private:
    sf::Vector2f worldSize   = { 1.0f, 1.0f };
    sf::Vector2f minimapPos  = { 0.0f, 0.0f };
    sf::Vector2f minimapSize = { 200.0f, 200.0f };

    sf::RectangleShape background;

private:
    sf::Vector2f worldToMinimap(const sf::Vector2f &worldPos) const;

    void drawBackground(sf::RenderWindow &window);
    void drawEntities(sf::RenderWindow &window, const EntityManager &entityManager, int clientId);
    void drawCameraRect(sf::RenderWindow &winndow, const sf::View &worldView);
    
public:
    MinimapRenderer();

    void setWorldSize(const sf::Vector2f &worldSize);
    void setPosition(const sf::Vector2f &position);
    void setSize(const sf::Vector2f &size);

    void render(
        sf::RenderWindow &window,
        const sf::View &worldView,
        const EntityManager &entityManager,
        int clientId
    );
};