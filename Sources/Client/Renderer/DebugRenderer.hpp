#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Shared/AABB.hpp"

class WorldCollision;
class ChunkSnapshot;
class DebugInfo;

class DebugRenderer {
private:
    sf::RenderWindow &window;
    bool enabled = false;

    std::vector<AABB> chunks;

    sf::View uiView;

    sf::Text informationText;

private:
    void drawWorldCollision(WorldCollision &worldCollision);
    void drawChunks();
    void drawInformation(const DebugInfo& debugInfo);
    void drawUI(const DebugInfo& debugInfo);

public:
    DebugRenderer(sf::RenderWindow &_window);

    void applySnapshot(ChunkSnapshot & chunkSnapshot);
    void setCamera();

    void toggle();
    bool isEnabled() const;

    void render(WorldCollision &worldCollision, const DebugInfo& debugInfo);
};