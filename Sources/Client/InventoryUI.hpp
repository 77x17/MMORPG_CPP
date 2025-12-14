#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Inventory;

class InventoryUI {
private:
    bool open = false;
    sf::Font font;
    
    sf::RectangleShape background;
    sf::Text title;

    int size    = 36;
    int columns = 9;

    int draggingIndex = -1;

    int getIndex(const sf::Vector2f &mousePosition);

    sf::RectangleShape equipmentBackground;
    sf::Text equipmentTitle;

public:
    InventoryUI();

    void toggle();
    bool isOpen() const;

    void handleLeftClick(const sf::Vector2f &mousePosition, const Inventory &inventory);
    std::pair<int, int> handleRelease(const sf::Vector2f &mousePosition);

    void draw(const Inventory &inventory, sf::RenderWindow &window);
};