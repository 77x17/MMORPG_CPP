#include "DebugRenderer.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include "EntityManager.hpp"
#include "RemotePlayer.hpp"
#include "WorldCollision.hpp"

#include <iostream>

DebugRenderer::DebugRenderer(sf::RenderWindow &_window) 
: window(_window) {
}

void DebugRenderer::toggle() {
    enabled = !enabled;
}

bool DebugRenderer::isEnabled() const {
    return enabled;
}

void DebugRenderer::drawWorldCollision(WorldCollision &worldCollision) {
    sf::RectangleShape colliderShape;
    colliderShape.setFillColor(sf::Color(0, 255, 0, 80));
    colliderShape.setOutlineThickness(1.0f);
    colliderShape.setOutlineColor(sf::Color::Green);

    for (const AABB &box : worldCollision.getColliders()) {
        colliderShape.setPosition(box.position);
        colliderShape.setSize(box.size);

        window.draw(colliderShape);
    }
}

void DebugRenderer::render(const EntityManager &entityManager, WorldCollision &worldCollision, int clientId) {
    drawWorldCollision(worldCollision);
}