#include "InventoryUI.hpp"

InventoryUI::InventoryUI() {
    background.setSize({ 400.0f, 300.0f });
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(sf::Color::White);

    background.setPosition(200.f, 150.0f);
    
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }

    title.setFont(font);
    title.setString("Inventory");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::White);
    title.setPosition(
        background.getPosition().x + 20.0f,
        background.getPosition().y + 10.0f
    );
}

void InventoryUI::toggle() {
    open = !open;
}

bool InventoryUI::isOpen() const {
    return open;
}

void InventoryUI::draw(sf::RenderWindow &window) {
    if (!open) return;

    window.draw(background);
    window.draw(title);

    sf::Text placeHolder("Empty (UI only)", font, 16);
    placeHolder.setPosition(
        background.getPosition().x + 20.0f,
        background.getPosition().y + 60.0f
    );

    window.draw(placeHolder);
}   