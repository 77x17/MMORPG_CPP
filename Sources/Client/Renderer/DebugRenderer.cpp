#include "Client/Renderer/DebugRenderer.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include "Client/World/WorldCollision.hpp"

#include "Client/Snapshots/ChunkSnapshot.hpp"

#include "Client/Renderer/DebugInfo.hpp"

#include "Client/Utils/Font.hpp"

#include <iostream>

DebugRenderer::DebugRenderer(sf::RenderWindow &_window) 
: window(_window) {
    informationText.setFont(Font::getFont());
    informationText.setCharacterSize(10.0f);
    informationText.setFillColor(sf::Color::White);
    informationText.setPosition(10.0f, 10.0f);
    
    setCamera();
}

void DebugRenderer::applySnapshot(ChunkSnapshot & chunkSnapshot) {
    chunks = chunkSnapshot.chunks;
}

void DebugRenderer::setCamera() {
    uiView = window.getDefaultView();
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
        colliderShape.setPosition(
            static_cast<int>(box.position.x),
            static_cast<int>(box.position.y)
        );
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
        chunkShape.setPosition(
            static_cast<int>(box.position.x),
            static_cast<int>(box.position.y)
        );
        chunkShape.setSize(box.size);

        window.draw(chunkShape);
    }
}

void DebugRenderer::drawInformation(const DebugInfo& debugInfo) {
    std::string informationString;
    informationString  =   "[FPS] | [X - Y]  : " + std::to_string(static_cast<int>(debugInfo.fps)) + " | " + std::to_string(static_cast<int>(debugInfo.playerPosition.x)) + " - " + std::to_string(static_cast<int>(debugInfo.playerPosition.y));
    informationString += "\n[TCP | UDP PING] : " + std::to_string(static_cast<int>(debugInfo.tcpPing)) + " ms | " + std::to_string(static_cast<int>(debugInfo.udpPing)) + " ms";
    informationString += "\n[PLAYERS]        : " + std::to_string(debugInfo.players);
    informationString += "\n[ENEMIES]        : " + std::to_string(debugInfo.enemies);
    informationString += "\n[DAMAGE ENTITY]  : " + std::to_string(debugInfo.damageEntities);
    
    informationText.setString(informationString);

    window.draw(informationText);
}

void DebugRenderer::drawUI(const DebugInfo& debugInfo) {
    sf::View oldView = window.getView();

    window.setView(uiView);

    drawInformation(debugInfo);

    window.setView(oldView);
}

void DebugRenderer::render(WorldCollision &worldCollision, const DebugInfo& debugInfo) {
    drawWorldCollision(worldCollision);

    if (enabled) {
        drawChunks();
     
        drawUI(debugInfo);
    }
}