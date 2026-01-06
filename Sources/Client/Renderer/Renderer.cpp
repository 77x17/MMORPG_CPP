#include "Client/Renderer/Renderer.hpp"

#include <SFML/Graphics.hpp>

#include <cmath>

#include "Client/Entities/EntityManager.hpp"

#include "Client/Inventory/Inventory.hpp"
#include "Client/Inventory/Equipment.hpp"

#include "Client/Snapshots/MouseSelectedSnapshot.hpp"

#include "Client/Utils/Font.hpp"

Renderer::Renderer(sf::RenderWindow &_window) : window(_window) {
    setCamera();

    buildBackground();
}

void Renderer::setCamera() {
    worldView.setSize(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );

    uiView = window.getDefaultView();

    inventoryUI.setStartPosition(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
}

InventoryUI &Renderer::getInventoryUI() {
    return inventoryUI;
}

void Renderer::updateCamera(const EntityManager &entityManager, int clientId) {
    const RemotePlayer *player = entityManager.getPlayer(clientId);
    if (player == nullptr) {
        return;
    }

    sf::Vector2f current = worldView.getCenter();
    sf::Vector2f target  = player->localPosition;

    sf::Vector2f smooth = current + (target - current) * 0.15f;
    smooth.x = std::floor(smooth.x);
    smooth.y = std::floor(smooth.y);

    worldView.setCenter(smooth);
}

void Renderer::drawEnemies(const EntityManager &entityManager) {
    sf::RectangleShape enemiesShape({ 40, 40 }); enemiesShape.setOrigin(20, 20);
    enemiesShape.setOutlineThickness(1.0f);
    for (auto &[id, enemy] : entityManager.getEnemies()) {
        enemiesShape.setPosition(enemy.localPosition);

        if (id == selectedId) {
            enemiesShape.setOutlineColor(sf::Color::Red);
        }
        else {
            enemiesShape.setOutlineColor(sf::Color::White);
        }

        if (enemy.hp == 0) {
            enemiesShape.setFillColor(sf::Color(0, 0, 0, 80));
        }
        else {
            enemiesShape.setFillColor(sf::Color::Black);
        }

        window.draw(enemiesShape);
    }
}

void Renderer::drawPlayers(const EntityManager &entityManager, int clientId) {
    sf::RectangleShape playerShape({ 40, 40 }); playerShape.setOrigin(20, 20);
    playerShape.setOutlineThickness(2.0f);
    
    for (auto &[id, player] : entityManager.getPlayers()) {
        playerShape.setPosition(player.localPosition);

        if (window.hasFocus() && player.id == clientId) {
            playerShape.setOutlineColor(sf::Color::Yellow);
        }
        else {
            playerShape.setOutlineColor(sf::Color::White);
        }

        if (player.hp == 0) {
            playerShape.setFillColor(sf::Color(0, 0, 255, 80));
        }
        else {
            playerShape.setFillColor(sf::Color::Blue);
        }

        window.draw(playerShape);
    }
}

void Renderer::drawDamageEntities(const EntityManager &entityManager, int clientId) {
    sf::CircleShape projectileShape(5); projectileShape.setOrigin(5, 5);
    for (auto &[id, projectile] : entityManager.getProjectiles()) {
        projectileShape.setPosition(projectile.localPosition);
        projectileShape.setFillColor(projectile.ownerId == clientId ? sf::Color::Yellow : sf::Color::White);

        window.draw(projectileShape);
    }

    sf::RectangleShape swordSlashShape;
    for (auto &[id, swordSlash] : entityManager.getSwordSlashs()) {
        swordSlashShape.setPosition(swordSlash.localPosition);
        swordSlashShape.setSize(swordSlash.size);
        // swordSlashShape.setOrigin(swordSlash.size / 2.0f);

        swordSlashShape.setFillColor(swordSlash.ownerId == clientId ? sf::Color::Yellow : sf::Color::White);

        window.draw(swordSlashShape);
    }
}

void Renderer::drawNametags(const EntityManager &entityManager) {
    sf::Text nametag;
    nametag.setFont(Font::getFont());
    nametag.setCharacterSize(12);

    for (auto &[id, enemy] : entityManager.getEnemies()) {
        nametag.setString(std::to_string(enemy.id) + ". HP: " + std::to_string(enemy.hp));
        sf::FloatRect bounds = nametag.getLocalBounds();
        nametag.setOrigin(bounds.left + bounds.width / 2.0f,
                          bounds.top  + bounds.height      );
        nametag.setPosition(enemy.localPosition - sf::Vector2f(0, 30));
        window.draw(nametag);
    }

    for (auto &[id, player] : entityManager.getPlayers()) {
        nametag.setString(std::to_string(player.id) + ". HP: " + std::to_string(player.hp));
        sf::FloatRect bounds = nametag.getLocalBounds();
        nametag.setOrigin(bounds.left + bounds.width / 2.0f,
                          bounds.top  + bounds.height      );
        nametag.setPosition(player.localPosition - sf::Vector2f(0, 30));
        window.draw(nametag);
    }
}

void Renderer::drawInventory(const Inventory &inventory, const Equipment &equipment) {
    inventoryUI.draw(inventory, equipment, window);
}

void Renderer::drawSelectedEntityInfo() {
    sf::Text label;
    label.setFont(Font::getFont());
    label.setCharacterSize(16);
    label.setPosition({ uiView.getSize().x / 2.0f, 20.0f});
    label.setString(selectedName + " Id: " + std::to_string(selectedId));
    sf::FloatRect bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);

    sf::Text healthbar;
    healthbar.setFont(Font::getFont());
    healthbar.setCharacterSize(16);
    healthbar.setPosition({ uiView.getSize().x / 2.0f, 40.0f});
    healthbar.setString(std::to_string(selectedHp) + " / " + std::to_string(selectedMaxHp));
    bounds = healthbar.getLocalBounds();
    healthbar.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);

    window.draw(label);
    window.draw(healthbar);
}

void Renderer::buildBackground() {
    backgroundTiles.clear();

    constexpr int MAP_WIDTH  = 50;
    constexpr int MAP_HEIGHT = 50;
    constexpr float TILE_SIZE = 64.0f;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            sf::RectangleShape tile({ TILE_SIZE, TILE_SIZE });
            tile.setPosition(
                static_cast<int>(x * TILE_SIZE),
                static_cast<int>(y * TILE_SIZE)
            );

            bool isDark = (x + y) % 2 == 0;
            tile.setFillColor(
                isDark ? sf::Color(60, 60, 60)
                       : sf::Color(80, 80, 80)
            );

            backgroundTiles.push_back(tile);
        }
    }
}

void Renderer::drawBackground() {
    for (auto& tile : backgroundTiles) {
        window.draw(tile);
    }
}

sf::Vector2f Renderer::getWorldPosition(const sf::Vector2i &pixel) const {
    return window.mapPixelToCoords(pixel, worldView);
}

void Renderer::applySnapshot(MouseSelectedSnapshot &mouseSelectedSnapshot) {
    selectedId    = mouseSelectedSnapshot.id;
    selectedName  = mouseSelectedSnapshot.name;
    selectedHp    = mouseSelectedSnapshot.hp;
    selectedMaxHp = mouseSelectedSnapshot.maxHp;
}

void Renderer::render(const EntityManager &entityManager, int clientId) {
    updateCamera(entityManager, clientId);
    window.setView(worldView);

    drawBackground();

    drawEnemies(entityManager);
    drawPlayers(entityManager, clientId);
    drawDamageEntities(entityManager, clientId);
    drawNametags(entityManager);
}

void Renderer::renderUI(const Inventory &inventory, const Equipment &equipment) {
    window.setView(uiView);
    drawInventory(inventory, equipment);

    if (selectedId != -1) {
        drawSelectedEntityInfo();
    }
}