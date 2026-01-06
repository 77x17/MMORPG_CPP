#include "Client/Renderer/MinimapRenderer.hpp"

#include "Client/Entities/EntityManager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/CircleShape.hpp>

MinimapRenderer::MinimapRenderer() {
    background.setFillColor(sf::Color(20, 20, 20, 180));
    background.setOutlineThickness(2.f);
    background.setOutlineColor(sf::Color::White);
}

void MinimapRenderer::setWorldSize(const sf::Vector2f& size) {
    worldSize = size;
}

void MinimapRenderer::setPosition(const sf::Vector2f& position) {
    minimapPos = position;
    background.setPosition(minimapPos);
}

void MinimapRenderer::setSize(const sf::Vector2f& size) {
    minimapSize = size;
    background.setSize(minimapSize);
}

// ======================================================

sf::Vector2f MinimapRenderer::worldToMinimap(const sf::Vector2f& worldPos) const {
    return {
        minimapPos.x + (worldPos.x / worldSize.x) * minimapSize.x,
        minimapPos.y + (worldPos.y / worldSize.y) * minimapSize.y
    };
}

// ======================================================

void MinimapRenderer::drawBackground(sf::RenderWindow& window) {
    window.draw(background);
}

// ======================================================

void MinimapRenderer::drawEntities(sf::RenderWindow& window, const EntityManager& entityManager, int clientId) {
    sf::CircleShape dot;
    dot.setOrigin(2.f, 2.f);

    // ---------- Players ----------
    dot.setRadius(3.f);

    for (auto& [id, player] : entityManager.getPlayers()) {
        dot.setPosition(worldToMinimap(player.localPosition));

        if (player.id == clientId)
            dot.setFillColor(sf::Color::Yellow);
        else
            dot.setFillColor(sf::Color::Blue);

        window.draw(dot);
    }

    // ---------- Enemies ----------
    dot.setRadius(2.f);
    dot.setOrigin(2.f, 2.f);
    dot.setFillColor(sf::Color::Red);

    for (auto& [id, enemy] : entityManager.getEnemies()) {
        dot.setPosition(worldToMinimap(enemy.localPosition));
        window.draw(dot);
    }
}

// ======================================================

void MinimapRenderer::drawCameraRect(sf::RenderWindow& window, const sf::View& worldView) {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(1.f);

    sf::Vector2f viewSize = worldView.getSize();
    sf::Vector2f viewPos  = worldView.getCenter() - viewSize / 2.f;

    sf::Vector2f size = {
        (viewSize.x / worldSize.x) * minimapSize.x,
        (viewSize.y / worldSize.y) * minimapSize.y
    };

    rect.setSize(size);
    rect.setPosition(worldToMinimap(viewPos));

    window.draw(rect);
}

// ======================================================

void MinimapRenderer::render(sf::RenderWindow& window, const sf::View& worldView, const EntityManager& entityManager, int clientId) {
    drawBackground(window);
    drawEntities(window, entityManager, clientId);
    drawCameraRect(window, worldView);
}
