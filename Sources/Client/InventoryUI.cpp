#include "InventoryUI.hpp"

#include <SFML/Window/Mouse.hpp>

#include "Inventory.hpp"
#include "Equipment.hpp"

InventoryUI::InventoryUI() {
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }

    background.setSize({ 
        (40.0f + 5.0f) * 9 + 5.0f, 
        (40.0f + 5.0f) * 5 + 5.0f 
    });
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

    equipmentBackground.setSize({ 
        (40.0f + 5.0f) * 5 + 5.0f, 
        (40.0f + 5.0f) * 5 + 5.0f 
    });
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

void InventoryUI::setStartPosition(const float &width, const float &height) {
    float subWidth  = (40.0f + 5.0f) * 9 + 5.0f + (40.0f + 5.0f) * 5 + 5.0f + 10.0f;
    float subHeight = (40.0f + 5.0f) * 5 + 5.0f;
    background.setPosition(
        width / 2 - subWidth / 2, 
        height / 2 - subHeight / 2
    );

    title.setPosition(
        background.getPosition().x + 5.0f,
        background.getPosition().y
    );

    equipmentBackground.setPosition(background.getPosition() + sf::Vector2f(background.getSize().x + 10.0f, 0.0f));

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

float InventoryUI::getPositionInventorySlotX(int index) {
    return background.getPosition().x +  5.0f + (index % columns) * (40.0f + 5.0f);
}

float InventoryUI::getPositionInventorySlotY(int index) {
    return background.getPosition().y + 50.0f + (index / columns) * (40.0f + 5.0f);
}

float InventoryUI::getPositionEquipmentSlotX(int index) {
    if (0 <= index && index <= 3) {
        return equipmentBackground.getPosition().x + 5.0f;
    }
    else if (4 <= index && index <= 7) {
        return equipmentBackground.getPosition().x + 5.0f + 4 * (40.0f + 5.0f);
    }
    
    return -1;
}

float InventoryUI::getPositionEquipmentSlotY(int index) {
    if (0 <= index && index <= 3) {
        return equipmentBackground.getPosition().y + 50.0f + (index    ) * (40.0f + 5.0f);
    }
    else if (4 <= index && index <= 7) {
        return equipmentBackground.getPosition().y + 50.0f + (index - 4) * (40.0f + 5.0f);
    }

    return -1;
}


int InventoryUI::getIndex(const sf::Vector2f &mousePosition) {
    sf::FloatRect itemBox;
    itemBox.width  = 40.0f;
    itemBox.height = 40.0f;
    for (int index = 0; index < size; ++index) {
        itemBox.left = getPositionInventorySlotX(index);
        itemBox.top  = getPositionInventorySlotY(index);

        if (itemBox.contains(mousePosition)) {
            return index;
        }
    }

    for (int index = 0; index <= 7; ++index) {
        itemBox.left = getPositionEquipmentSlotX(index);
        itemBox.top  = getPositionEquipmentSlotY(index);
        
        if (itemBox.contains(mousePosition)) {
            return size + index;
        }
    }

    return -1;
}

void InventoryUI::handleLeftClick(const sf::Vector2f &mousePosition, const Inventory &inventory, const Equipment &equipment) {
    int index = getIndex(mousePosition);
    
    if (index < size) {
        if (draggingIndex.second == -1 && index != -1 && inventory[index].itemId != -1) {
            draggingIndex.first  = 0;
            draggingIndex.second = index;
        }
        else {
            draggingIndex = { -1, -1 };
        }
    }
    else {
        index -= size;
        if (draggingIndex.second == -1 && index != -1 && equipment[index].itemId != -1) {
            draggingIndex.first  = 1;
            draggingIndex.second = index;
        }
        else {
            draggingIndex = { -1, -1 };
        }
    }
}

std::pair<std::pair<int, int>, std::pair<int, int>> InventoryUI::handleRelease(const sf::Vector2f &mousePosition) {
    int index = getIndex(mousePosition);

    if (index < size) {
        if (index == -1 || draggingIndex.second == -1 || (draggingIndex.first == 0 && draggingIndex.second == index)) {
            return { draggingIndex = { -1, -1 }, { -1, -1} };
        }

        std::pair<int, int> temporary = draggingIndex;
        draggingIndex = { -1, -1 };
        return { temporary, { 0, index } };
    }
    else {
        index -= size;
        if (index == -1 || draggingIndex.second == -1 || (draggingIndex.first == 1 && draggingIndex.second == index)) {
            return { draggingIndex = { -1, -1 }, { -1, -1} };
        }

        std::pair<int, int> temporary = draggingIndex;
        draggingIndex = { -1, -1 };
        return { temporary, { 1, index } };
    }

    return { draggingIndex = { -1, -1 }, { -1, -1 } };
}

void InventoryUI::draw(const Inventory &inventory, const Equipment &equipment, sf::RenderWindow &window) {
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
            getPositionInventorySlotX(index),
            getPositionInventorySlotY(index)
        });

        window.draw(itemBox);

        if (inventory[index].itemId != -1 && (index != draggingIndex.second || draggingIndex.first != 0)) {
            itemId.setString(std::to_string(inventory[index].itemId));
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
            getPositionEquipmentSlotX(index),
            getPositionEquipmentSlotY(index)
        });

        window.draw(itemBox);

        boxLabel.setString(label[index]);
        boxLabel.setPosition(itemBox.getPosition() + sf::Vector2f(itemBox.getSize().x + 5.0f, 0.0f));

        window.draw(boxLabel);

        if (equipment[index].itemId != -1 && (index != draggingIndex.second || draggingIndex.first != 1)) {
            itemId.setString(std::to_string(equipment[index].itemId));
            itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));
            
            window.draw(itemId);
        }
    }

    for (int index = 4; index < 8; ++index) {
        itemBox.setPosition({
            getPositionEquipmentSlotX(index),
            getPositionEquipmentSlotY(index)
        });

        window.draw(itemBox);

        boxLabel.setString(label[index]);
        boxLabel.setPosition(itemBox.getPosition() + sf::Vector2f(-5.0f, 40.0f - 3.0f));
        sf::FloatRect bounds = boxLabel.getLocalBounds();
        boxLabel.setOrigin(bounds.left + bounds.width, bounds.top + bounds.height);

        window.draw(boxLabel);
        
        if (equipment[index].itemId != -1 && (index != draggingIndex.second || draggingIndex.first != 1)) {
            itemId.setString(std::to_string(equipment[index].itemId));
            itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));
            
            window.draw(itemId);
        }
    }

    if (draggingIndex.first != -1 && draggingIndex.second != -1) {
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

        itemBox.setPosition(mouseWorldPos - itemBox.getSize());

        window.draw(itemBox);

        itemId.setString(std::to_string(draggingIndex.first == 0 ? inventory[draggingIndex.second].itemId : equipment[draggingIndex.second].itemId));
        itemId.setPosition(itemBox.getPosition() + sf::Vector2f( 5.0f, 5.0f ));
        itemId.setFillColor(sf::Color::Yellow);

        window.draw(itemId);
    }
}   