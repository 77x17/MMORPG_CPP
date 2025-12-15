#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

#include "EntityManager.hpp"
#include "InventoryUI.hpp"
#include "Inventory.hpp"

class Renderer {
private:
    sf::RenderWindow &window;
    sf::Font font;

    InventoryUI inventoryUI;

public:
    Renderer(sf::RenderWindow &_window);
    
    InventoryUI &getInventoryUI();

    void render(const EntityManager &entityManager, const Inventory &inventory, const Equipment &equipment, int myId);
    
    void drawWorld(const EntityManager &entityManager, int myId);
    void drawPlayers(const EntityManager &entityManager, int myId);
};