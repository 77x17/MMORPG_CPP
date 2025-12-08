#pragma once

#include <SFML/Graphics.hpp>

#include "EntityManager.hpp"

class Renderer {
private:
    sf::RenderWindow &window;
    sf::Font font;

public:
    Renderer(sf::RenderWindow &_window);

    void render(const EntityManager &entityManager, int myId);
};