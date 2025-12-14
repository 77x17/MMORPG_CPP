#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class InventoryUI {
private:
    bool open = false;
    sf::RectangleShape background;
    sf::Font font;
    sf::Text title;

public:
    InventoryUI();

    void toggle();
    bool isOpen() const;

    void draw(sf::RenderWindow &window);

};