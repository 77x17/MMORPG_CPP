#include "InventoryUI.hpp"

#include <SFML/Window/Mouse.hpp>

#include "Inventory.hpp"

InventoryUI::InventoryUI() {
    background.setSize({ 400.0f + 10.0f, 300.0f });
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(sf::Color::White);

    background.setPosition(200.0f, 150.0f);
    
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }

    title.setFont(font);
    title.setString("Inventory");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::White);
    title.setPosition(
        background.getPosition().x + 5.0f,
        background.getPosition().y
    );
}

void InventoryUI::toggle() {
    open = !open;
}

bool InventoryUI::isOpen() const {
    return open;
}

int InventoryUI::getIndex(const sf::Vector2f &mousePosition) {
    sf::FloatRect itemBox;
    itemBox.width  = 40.0f;
    itemBox.height = 40.0f;
    for (int index = 0; index < size; ++index) {
        itemBox.left = background.getPosition().x + 5.0f  + (index % columns) * (40.0f + 5.0f);
        itemBox.top  = background.getPosition().y + 40.0f + (index / columns) * (40.0f + 5.0f);

        if (itemBox.contains(mousePosition)) {
            return index;
        }
    }

    return -1;
}

void InventoryUI::handleLeftClick(const sf::Vector2f &mousePosition) {
    int index = getIndex(mousePosition);
    
    if (draggingIndex == -1) {
        draggingIndex = index;
    }
}

std::pair<int, int> InventoryUI::handleRelease(const sf::Vector2f &mousePosition) {
    int index = getIndex(mousePosition);

    if (index == -1 || draggingIndex == -1 || draggingIndex == index) {
        draggingIndex = -1;
        return std::make_pair(-1, -1);
    }

    std::pair<int, int> result = std::make_pair(draggingIndex, index);
    draggingIndex = -1;

    return result;
}

void InventoryUI::draw(const Inventory &inventory, sf::RenderWindow &window) {
    if (!open) return;

    window.draw(background);
    window.draw(title);

    sf::RectangleShape itemBox;
    itemBox.setSize({ 40.0f, 40.0f });
    itemBox.setFillColor(sf::Color(0, 0, 0, 120));
    itemBox.setOutlineThickness(1.0f);
    itemBox.setOutlineColor(sf::Color::White);

    sf::Text itemId;
    itemId.setFont(font);
    itemId.setCharacterSize(10.0f);
    itemId.setFillColor(sf::Color::White);
    for (int index = 0; index < size; ++index) {
        itemBox.setPosition({
            background.getPosition().x + 5.0f  + (index % columns) * (40.0f + 5.0f),
            background.getPosition().y + 40.0f + (index / columns) * (40.0f + 5.0f)
        });

        window.draw(itemBox);

        if (index != draggingIndex) {
            itemId.setString(std::to_string(inventory.getSlots()[index].itemId));
            itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));
        }
        
        window.draw(itemId);
    }

    if (draggingIndex != -1) {
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

        itemBox.setPosition(mouseWorldPos - itemBox.getSize());

        window.draw(itemBox);

        itemId.setString(std::to_string(inventory.getSlots()[draggingIndex].itemId));
        itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));

        window.draw(itemId);
    }
}   