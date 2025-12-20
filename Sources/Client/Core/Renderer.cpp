#include "Client/Core/Renderer.hpp"

#include <SFML/Graphics.hpp>

#include <cmath>

#include "Client/Systems/Entity/EntityManager.hpp"
#include "Client/Inventory/Inventory.hpp"
#include "Client/Inventory/Equipment.hpp"

Renderer::Renderer(sf::RenderWindow &_window) : window(_window) {
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }

    setCamera();

    buildBackground();
}

void Renderer::setCamera() {
    worldView.setSize(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    // worldView.setCenter(0.0f, 0.0f);

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
    if (not entityManager.findPlayer(clientId)) {
        return;
    }

    const RemotePlayer &player = entityManager.getPlayer(clientId);

    sf::Vector2f current = worldView.getCenter();
    sf::Vector2f target  = player.localPosition;

    sf::Vector2f smooth = current + (target - current) * 0.15f;
    smooth.x = std::floor(smooth.x);
    smooth.y = std::floor(smooth.y);

    worldView.setCenter(smooth);
}

void Renderer::drawEnemies(const EntityManager &entityManager) {
    sf::RectangleShape enemiesShape({ 40, 40 }); enemiesShape.setOrigin(20, 20);
    for (auto &[id, enemy] : entityManager.getEnemies()) {
        enemiesShape.setPosition(enemy.localPosition);
        enemiesShape.setFillColor(sf::Color::Black);

        enemiesShape.setOutlineColor(sf::Color::White);
        enemiesShape.setOutlineThickness(1.0f);

        window.draw(enemiesShape);
    }
}

void Renderer::drawPlayers(const EntityManager &entityManager, int clientId) {
    sf::RectangleShape playerShape({ 40, 40 }); playerShape.setOrigin(20, 20);
    for (auto &[id, player] : entityManager.getPlayers()) {
        playerShape.setPosition(player.localPosition);
        playerShape.setFillColor(player.id == clientId ? sf::Color::Blue : sf::Color::Red);

        if (window.hasFocus() && player.id == clientId) {
            playerShape.setOutlineColor(sf::Color::Yellow);
            playerShape.setOutlineThickness(1.0f);
        }
        else {
            playerShape.setOutlineThickness(0.0f);
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
    nametag.setFont(font);
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

void Renderer::drawUI(const Inventory &inventory, const Equipment &equipment) {
    inventoryUI.draw(inventory, equipment, window);
}

void Renderer::buildBackground() {
    backgroundTiles.clear();

    constexpr int MAP_WIDTH  = 50;
    constexpr int MAP_HEIGHT = 50;
    constexpr float TILE_SIZE = 64.0f;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            sf::RectangleShape tile({ TILE_SIZE, TILE_SIZE });
            tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);

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
    drawUI(inventory, equipment);

    sf::Text position;
    position.setFont(font);
    position.setCharacterSize(10.0f);
    position.setFillColor(sf::Color::White);
    position.setPosition(10.0f, 10.0f);
    position.setString(std::to_string(static_cast<int>(worldView.getCenter().x)) + " " + std::to_string(static_cast<int>(worldView.getCenter().y)));

    window.draw(position);
}