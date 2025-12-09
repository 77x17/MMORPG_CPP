#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

#include "EntityManager.hpp"

class Renderer {
private:
    sf::RenderWindow &window;
    sf::Font font;

public:
    Renderer(sf::RenderWindow &_window);

    void render(const EntityManager &entityManager, int myId);
};