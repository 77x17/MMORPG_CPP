#include "Server/Renderer/Renderer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "Server/Debug/DebugSystem.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

#include "Server/Utils/Font.hpp"

Renderer::Renderer(sf::RenderWindow &_window) 
: window(_window) {
    players.box .setFillColor(sf::Color::Transparent);
    enemies.box .setFillColor(sf::Color::Transparent);
    entities.box.setFillColor(sf::Color::Transparent);
    logs.box    .setFillColor(sf::Color::Transparent);

    players.box .setOutlineThickness(2.0f);
    enemies.box .setOutlineThickness(2.0f);
    entities.box.setOutlineThickness(2.0f);
    logs.box    .setOutlineThickness(2.0f);

    players.box .setOutlineColor(sf::Color::White);
    enemies.box .setOutlineColor(sf::Color::White);
    entities.box.setOutlineColor(sf::Color::White);
    logs.box    .setOutlineColor(sf::Color::White);

    players.label .setFont(Font::getFont());
    enemies.label .setFont(Font::getFont());
    entities.label.setFont(Font::getFont());
    logs.label    .setFont(Font::getFont());

    players.label .setCharacterSize(16);
    enemies.label .setCharacterSize(16);
    entities.label.setCharacterSize(16);
    logs.label    .setCharacterSize(16);

    players.label .setFillColor(sf::Color::White);
    enemies.label .setFillColor(sf::Color::White);
    entities.label.setFillColor(sf::Color::White);
    logs.label    .setFillColor(sf::Color::White);

    players.label .setString("Clients / Players");
    enemies.label .setString("Enemies");
    entities.label.setString("Entities");
    logs.label    .setString("Logs");

    players.text .setFont(Font::getFont());
    enemies.text .setFont(Font::getFont());
    entities.text.setFont(Font::getFont());
    logs.text    .setFont(Font::getFont());

    players.text .setCharacterSize(10);
    enemies.text .setCharacterSize(10);
    entities.text.setCharacterSize(10);
    logs.text    .setCharacterSize(10);

    players.text .setFillColor(sf::Color::White);
    enemies.text .setFillColor(sf::Color::White);
    entities.text.setFillColor(sf::Color::White);
    logs.text    .setFillColor(sf::Color::White);

    resize();
}

void Renderer::resize() {
    int width  = window.getSize().x;
    int height = window.getSize().y;

    players .box.setSize({ (width - padding * 3) / 2.0f, (height - padding * 4) / 3.0f });
    enemies .box.setSize(players.box.getSize());
    entities.box.setSize(players.box.getSize());
    logs    .box.setSize({ (width - padding * 3) / 2.0f, height - padding * 2 });

    players .box.setPosition({ padding, padding });
    enemies .box.setPosition({ padding, players.box.getPosition().y + players.box.getSize().y + padding });
    entities.box.setPosition({ padding, enemies.box.getPosition().y + enemies.box.getSize().y + padding });
    logs    .box.setPosition({ players.box.getPosition().x + players.box.getSize().x + padding, padding });

    players .label.setPosition(players.box .getPosition() - sf::Vector2f( 0.0f, players .label.getCharacterSize() + 5 ));
    enemies .label.setPosition(enemies.box .getPosition() - sf::Vector2f( 0.0f, enemies .label.getCharacterSize() + 5 ));
    entities.label.setPosition(entities.box.getPosition() - sf::Vector2f( 0.0f, entities.label.getCharacterSize() + 5 ));
    logs    .label.setPosition(logs.box    .getPosition() - sf::Vector2f( 0.0f, logs    .label.getCharacterSize() + 5 ));

    players .setupView(window.getSize());
    enemies .setupView(window.getSize());
    entities.setupView(window.getSize());
    logs    .setupView(window.getSize());

    players .updateScrollbar();
    enemies .updateScrollbar();
    entities.updateScrollbar();
    logs    .updateScrollbar();
}
    
void Renderer::handleScroll(const sf::Event &event) {
    players .handleScroll(event);
    enemies .handleScroll(event);
    entities.handleScroll(event);
    logs    .handleScroll(event);
}

void Renderer::handleMousePressed(const sf::Event &event) {
    players .handleMousePressed(event);
    enemies .handleMousePressed(event);
    entities.handleMousePressed(event);
    logs    .handleMousePressed(event);
}

void Renderer::handleMouseReleased(const sf::Event &event) {
    players .handleMouseReleased(event);
    enemies .handleMouseReleased(event);
    entities.handleMouseReleased(event);
    logs    .handleMouseReleased(event);
}

void Renderer::handleMouseMoved(const sf::Event &event) {
    players .handleMouseMoved(event);
    enemies .handleMouseMoved(event);
    entities.handleMouseMoved(event);
    logs    .handleMouseMoved(event);
}

void Renderer::drawArea(ScrollArea &area) {
    window.draw(area.box);
    window.draw(area.label);

    sf::View old = window.getView();
    window.setView(area.view);

    area.text.setPosition(
        area.box.getPosition().x + 3.0f,
        area.box.getPosition().y + 3.0f - area.getScrollOffSet()
    );

    window.draw(area.text);
    window.setView(old);
    
    area.drawScrollbar(window);
}

void Renderer::render(const DebugSnapshot &snapshot) {
    players .text.setString(snapshot.players);
    enemies .text.setString(snapshot.enemies);
    entities.text.setString(snapshot.entities);
    logs    .text.setString(snapshot.logs);

    players .onContentChanged();
    enemies .onContentChanged();
    entities.onContentChanged();
    logs    .onContentChanged();
    
    drawArea(players);
    drawArea(enemies);
    drawArea(entities);
    drawArea(logs);
}