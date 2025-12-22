#include "Server/Renderer/ScrollArea.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

float ScrollArea::getScrollOffSet() const {
    return scrollOffset;
}

void ScrollArea::setupView(const sf::Vector2u &windowSize) {
    sf::Vector2f position = box.getPosition();
    sf::Vector2f size     = box.getSize();

    view.reset(sf::FloatRect(position, size));
    view.setViewport(sf::FloatRect(
        position.x / windowSize.x, 
        position.y / windowSize.y, 
        size.x / windowSize.x,
        size.y / windowSize.y
    ));
}

void ScrollArea::handleScroll(const sf::Event &event) {
    sf::Vector2f mouse(
        event.mouseWheelScroll.x,
        event.mouseWheelScroll.y
    );

    if (box.getGlobalBounds().contains(mouse) == false) {
        return;
    }

    autoScroll = false;

    scrollOffset -= event.mouseWheelScroll.delta * scrollSpeed;

    float textHeight = text.getLocalBounds().height + text.getLocalBounds().top;
    float viewHeight = box.getSize().y;

    float maxScroll = std::max(0.0f, textHeight - viewHeight);
    scrollOffset = std::clamp(scrollOffset, 0.0f, maxScroll);
    
    updateScrollbar();
}

void ScrollArea::onContentChanged() {
    float textHeight = text.getLocalBounds().height + text.getLocalBounds().top;
    float viewHeight = box.getSize().y;

    float maxScroll = std::max(0.0f, textHeight - viewHeight);

    if (autoScroll) {
        scrollOffset = maxScroll;
    }

    updateScrollbar();
}

void ScrollArea::updateScrollbar() {
    float contentHeight = text.getLocalBounds().height + text.getLocalBounds().top;
    float viewHeight = box.getSize().y;

    if (contentHeight <= viewHeight) {
        scrollbarThumb.setSize({ 0.0f, 0.0f });
        return;
    }

    float maxScroll = contentHeight - viewHeight;

    float barWidth = 6.0f;
    scrollbarTrack.setSize({ barWidth, viewHeight });
    scrollbarTrack.setFillColor(sf::Color(80, 80, 80, 200));
    scrollbarTrack.setPosition(
        box.getPosition().x + box.getSize().x - barWidth,
        box.getPosition().y
    );

    float ratio = viewHeight / contentHeight;
    float thumbHeight = viewHeight * ratio;
    scrollbarThumb.setSize({ barWidth, thumbHeight });
    scrollbarThumb.setFillColor(sf::Color(160, 160, 160));

    float scrollRatio = scrollOffset / maxScroll;
    float thumbY = scrollbarTrack.getPosition().y + scrollRatio * (viewHeight - thumbHeight);

    scrollbarThumb.setPosition(
        scrollbarTrack.getPosition().x,
        thumbY
    );
}

void ScrollArea::drawScrollbar(sf::RenderWindow &window) {
    if (scrollbarThumb.getSize().y <= 0) return;

    window.draw(scrollbarTrack);
    window.draw(scrollbarThumb);
}

void ScrollArea::handleMousePressed(const sf::Event &event) {
    if (event.mouseButton.button == sf::Mouse::Right) {
        autoScroll = true;
        onContentChanged();
    }
    
    if (event.mouseButton.button != sf::Mouse::Left)
        return;

    sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);

    if (scrollbarThumb.getGlobalBounds().contains(mouse)) {
        draggingScrollbar = true;
        dragOffsetY = mouse.y - scrollbarThumb.getPosition().y;
    }

    autoScroll = false;
}

void ScrollArea::handleMouseReleased(const sf::Event &event) {
    draggingScrollbar = false;
}

void ScrollArea::handleMouseMoved(const sf::Event &event) {
    if (!draggingScrollbar) return;

    float contentHeight = text.getLocalBounds().height + text.getLocalBounds().top;
    float viewHeight = box.getSize().y;

    float maxScroll = contentHeight - viewHeight;
    if (maxScroll <= 0) return;

    float trackTop = scrollbarTrack.getPosition().y;
    float trackHeight = scrollbarTrack.getSize().y;
    float thumbHeight = scrollbarThumb.getSize().y;

    float mouseY = event.mouseMove.y;
    float newThumbY = mouseY - dragOffsetY;

    newThumbY = std::clamp(
        newThumbY,
        trackTop,
        trackTop + trackHeight - thumbHeight
    );

    scrollbarThumb.setPosition(
        scrollbarThumb.getPosition().x,
        newThumbY
    );

    float ratio = (newThumbY - trackTop) / (trackHeight - thumbHeight);

    scrollOffset = ratio * maxScroll;
}