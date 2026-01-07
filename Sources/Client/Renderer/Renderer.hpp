#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class EntityManager;
class MouseSelectedSnapshot;

#include "Client/UI/InventoryUI.hpp"
#include "Client/Renderer/MinimapRenderer.hpp"

class Renderer {
private:
    sf::RenderWindow &window;

    sf::View worldView;
    sf::View uiView;

    InventoryUI inventoryUI;

    std::vector<sf::RectangleShape> backgroundTiles;

    // === Mouse Selected ===
    int selectedEntityId = -1;
    int selectedEnemyId = -1;
    std::string selectedName;
    int selectedHp;
    int selectedMaxHp;

    MinimapRenderer minimap;

private:
    void updateCamera(const EntityManager &entityManager, int clientId);

    void drawEnemies(const EntityManager &entityManager);
    void drawPlayers(const EntityManager &entityManager, int clientId);
    void drawDamageEntities(const EntityManager &entityManager, int clientId);
    void drawNametags(const EntityManager &entityManager);
    void drawInventory(const Inventory &inventory, const Equipment& equipment);
    
    void buildBackground();
    void drawBackground();

    void drawSelectedEntityInfo();

public:
    Renderer(sf::RenderWindow &_window);
    
    void setCamera();

    InventoryUI & getInventoryUI();

    void render(const EntityManager &entityManager, int clientId);

    // === Mouse Selected ===
    sf::Vector2f getWorldPosition(const sf::Vector2i &pixel) const;
    void applySnapshot(MouseSelectedSnapshot &mouseSelectedSnapshot);

    void renderUI(const EntityManager &entityManager, int clientId, const Inventory &inventory, const Equipment &equipment);
};