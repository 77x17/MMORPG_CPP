#pragma once

namespace sf { class RenderWindow; }

class DebugSnapshot;

#include "Server/Renderer/ScrollArea.hpp"

class Renderer {
private:
    sf::RenderWindow &window;

    const float padding = 30.0f;

    ScrollArea players;
    ScrollArea enemies;
    ScrollArea entities;
    ScrollArea logs;

public:
    Renderer(sf::RenderWindow &_window);

    void resize();

    void handleScroll(const sf::Event &event);
    void handleMousePressed(const sf::Event &event);
    void handleMouseReleased(const sf::Event &event);
    void handleMouseMoved(const sf::Event &event);

    void drawArea(ScrollArea &area);

    void render(const DebugSnapshot &snapshot);
};