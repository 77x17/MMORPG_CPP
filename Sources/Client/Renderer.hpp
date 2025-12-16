#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "EntityManager.hpp"
#include "InventoryUI.hpp"
#include "Inventory.hpp"

class Renderer {
private:
    sf::RenderWindow &window;
    sf::Font font;

    sf::View worldView;
    sf::View uiView;

    InventoryUI inventoryUI;

    std::vector<sf::RectangleShape> backgroundTiles;

private:
    void updateCamera(const EntityManager &entityManager, int clientId);

    void drawPlayers(const EntityManager &entityManager, int clientId);
    void drawWorld(const EntityManager &entityManager, int clientId);
    void drawUI(const Inventory &inventory, const Equipment& equipment);
    
    void buildBackground();
    void drawBackground();

public:
    Renderer(sf::RenderWindow &_window);
    
    void setCamera();

    InventoryUI &getInventoryUI();

    void render(const EntityManager &entityManager, const Inventory &inventory, const Equipment &equipment, int clientId);
};