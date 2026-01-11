#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace sf { class RenderWindow; }
namespace sf { class Event; }

struct QuestState;

class QuestRenderer {
private:
    sf::Vector2f questPosition = { 0.0f, 0.0f };
    sf::Vector2f questSize     = { 200.0f, 400.0f };

    sf::RectangleShape background;

private:
    void drawBackground(sf::RenderWindow &window);
    void drawQuests(sf::RenderWindow &window, const std::vector<QuestState> &questStates);

public:
    QuestRenderer();

    void setPosition(const sf::Vector2f &position);
    void setSize(const sf::Vector2f &size);

    void render(sf::RenderWindow &window, const std::vector<QuestState> &questStates);
};