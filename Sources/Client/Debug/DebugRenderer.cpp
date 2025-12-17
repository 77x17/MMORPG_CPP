#include "Client/Debug/DebugRenderer.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include "Client/Entities/RemotePlayer.hpp"
#include "Client/Systems/Entity/EntityManager.hpp"
#include "Client/Systems/World/WorldCollision.hpp"
#include "Client/Snapshots/ChunkSnapshot.hpp"

#include <iostream>

DebugRenderer::DebugRenderer(sf::RenderWindow &_window) 
: window(_window) {
}

void DebugRenderer::applySnapshot(ChunkSnapshot & chunkSnapshot) {
    chunks = chunkSnapshot.chunks;
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

void DebugRenderer::drawChunks() {
    sf::RectangleShape chunkShape;
    chunkShape.setFillColor(sf::Color::Transparent);
    chunkShape.setOutlineThickness(1.0f);
    chunkShape.setOutlineColor(sf::Color::Red);
    for (const AABB &box : chunks) {
        chunkShape.setPosition(box.position);
        chunkShape.setSize(box.size);

        window.draw(chunkShape);
    }
}

void DebugRenderer::render(const EntityManager &entityManager, WorldCollision &worldCollision, int clientId) {
    drawChunks();
    drawWorldCollision(worldCollision);
}