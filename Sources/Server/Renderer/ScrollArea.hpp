#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>

#include <SFML/Window/Event.hpp>

namespace sf { 
    class RenderWindow; 
    class Event;
}

class ScrollArea {
private:
    float scrollOffset = 0.0f;
    float scrollSpeed = 20.0f;
    
    bool autoScroll = true;
    bool draggingScrollbar = false;
    float dragOffsetY = 0.0f;

    sf::RectangleShape scrollbarTrack;
    sf::RectangleShape scrollbarThumb;
    
public:
    sf::RectangleShape box;
    sf::Text label;
    sf::Text text;
    sf::View view;

    float getScrollOffSet() const;

    void setupView(const sf::Vector2u &windowSize);
    void handleScroll(const sf::Event &event);
    void onContentChanged();

    void updateScrollbar();
    void drawScrollbar(sf::RenderWindow &window);
    void handleMousePressed(const sf::Event &event);
    void handleMouseReleased(const sf::Event &event);
    void handleMouseMoved(const sf::Event &event);
};