#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Inventory;
class Equipment;

class InventoryUI {
private:
    bool open = false;
    sf::Font font;
    
    sf::RectangleShape background;
    sf::Text title;
    
    sf::RectangleShape equipmentBackground;
    sf::Text equipmentTitle;

    int size    = 36;
    int columns = 9;

    // first is 0: inventory | 1: equipment --- second is index
    std::pair<int, int> draggingIndex = { -1, -1 };

    float getPositionInventorySlotX(int index);
    float getPositionInventorySlotY(int index);
    float getPositionEquipmentSlotX(int index);
    float getPositionEquipmentSlotY(int index);
    int getIndex(const sf::Vector2f &mousePosition);

public:
    InventoryUI();

    void toggle();
    bool isOpen() const;

    void handleLeftClick(const sf::Vector2f &mousePosition, const Inventory &inventory, const Equipment &equipment);
    std::pair<std::pair<int, int>, std::pair<int, int>> handleRelease(const sf::Vector2f &mousePosition);

    void draw(const Inventory &inventory, const Equipment &equipment, sf::RenderWindow &window);
};