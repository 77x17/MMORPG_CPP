#include "InventoryUI.hpp"

#include <SFML/Window/Mouse.hpp>

#include "Inventory.hpp"

InventoryUI::InventoryUI() {
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }

    background.setSize({ (40.0f + 5.0f) * 9 + 5.0f, (40.0f + 5.0f) * 5 + 5.0f });
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(sf::Color::White);
    background.setPosition(75.0f, 150.0f);

    title.setFont(font);
    title.setString("Inventory");
    title.setCharacterSize(20);
    title.setFillColor(sf::Color::White);
    title.setPosition(
        background.getPosition().x + 5.0f,
        background.getPosition().y
    );

    equipmentBackground.setSize({ (40.0f + 5.0f) * 5 + 5.0f, (40.0f + 5.0f) * 5 + 5.0f });
    equipmentBackground.setFillColor(sf::Color(0, 0, 0, 180));
    equipmentBackground.setOutlineThickness(2.0f);
    equipmentBackground.setOutlineColor(sf::Color::White);
    equipmentBackground.setPosition(background.getPosition() + sf::Vector2f(background.getSize().x + 10.0f, 0.0f));

    equipmentTitle.setFont(font);
    equipmentTitle.setString("Equipment");
    equipmentTitle.setCharacterSize(20);
    equipmentTitle.setFillColor(sf::Color::White);
    equipmentTitle.setPosition(
        equipmentBackground.getPosition().x + 5.0f,
        equipmentBackground.getPosition().y
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
        itemBox.left = background.getPosition().x +  5.0f + (index % columns) * (40.0f + 5.0f);
        itemBox.top  = background.getPosition().y + 50.0f + (index / columns) * (40.0f + 5.0f);

        if (itemBox.contains(mousePosition)) {
            return index;
        }
    }

    return -1;
}

void InventoryUI::handleLeftClick(const sf::Vector2f &mousePosition, const Inventory &inventory) {
    int index = getIndex(mousePosition);
    
    if (draggingIndex == -1 && index != -1 && inventory.getSlots()[index].itemId != -1) {
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

    window.draw(equipmentBackground);
    window.draw(equipmentTitle);

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
            background.getPosition().x +  5.0f + (index % columns) * (40.0f + 5.0f),
            background.getPosition().y + 50.0f + (index / columns) * (40.0f + 5.0f)
        });

        window.draw(itemBox);

        if (inventory.getSlots()[index].itemId != -1 && index != draggingIndex) {
            itemId.setString(std::to_string(inventory.getSlots()[index].itemId));
            itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));
            
            window.draw(itemId);
        }
    }

    sf::Text boxLabel;
    boxLabel.setFont(font);
    boxLabel.setCharacterSize(13.0f);
    boxLabel.setFillColor(sf::Color::White);
    std::string label[] = { "Helmet", "Chestplate", "Leggings", "Boots", "Weapon", "Shield", "Ring", "Amulet" };
    for (int index = 0; index < 4; ++index) {
        itemBox.setPosition({
            equipmentBackground.getPosition().x +  5.0f,
            equipmentBackground.getPosition().y + 50.0f + index * (40.0f + 5.0f)
        });

        window.draw(itemBox);

        boxLabel.setString(label[index]);
        boxLabel.setPosition(itemBox.getPosition() + sf::Vector2f(itemBox.getSize().x + 5.0f, 0.0f));

        window.draw(boxLabel);
    }

    for (int index = 4; index < 8; ++index) {
        itemBox.setPosition({
            equipmentBackground.getPosition().x +  5.0f + 4 * (40.0f + 5.0f),
            equipmentBackground.getPosition().y + 50.0f + (index - 4) * (40.0f + 5.0f)
        });

        window.draw(itemBox);

        boxLabel.setString(label[index]);
        boxLabel.setPosition(itemBox.getPosition() + sf::Vector2f(-5.0f, 40.0f - 3.0f));
        sf::FloatRect bounds = boxLabel.getLocalBounds();
        boxLabel.setOrigin(bounds.left + bounds.width, bounds.top + bounds.height);

        window.draw(boxLabel);
    }

    if (draggingIndex != -1) {
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

        itemBox.setPosition(mouseWorldPos - itemBox.getSize());

        window.draw(itemBox);

        itemId.setString(std::to_string(inventory.getSlots()[draggingIndex].itemId));
        itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));
        itemId.setFillColor(sf::Color::Yellow);

        window.draw(itemId);
    }
}   