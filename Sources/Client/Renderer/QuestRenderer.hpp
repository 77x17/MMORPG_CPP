#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <unordered_set>
#include <string>

#include "Client/Quests/QuestTypes.hpp"

namespace sf { class RenderWindow; }
namespace sf { class Event; }

struct QuestState;

enum class QuestUIState {
    TrackingOnly,
    FullWindow
};

class QuestRenderer {
private:
    sf::Vector2f questPosition = { 0.0f, 0.0f };
    sf::Vector2f questSize     = { 200.0f, 400.0f };

    sf::RectangleShape background;

    QuestUIState uiState = QuestUIState::TrackingOnly;
    QuestID selectedQuest = -1;
    std::unordered_set<QuestID> trackedQuests;

    sf::RectangleShape buttonTrack;

private:
    void drawBackground(sf::RenderWindow &window);
    void drawQuests(sf::RenderWindow &window, const std::vector<QuestState> &questStates);

    void drawTrackingPanel(sf::RenderWindow &window, const std::vector<QuestState> &questStates);
    void drawFullWindow(sf::RenderWindow &window, const std::vector<QuestState> &questStates);

    void drawQuestList(sf::RenderWindow &window, const std::vector<QuestState> &questStates);
    void drawQuestDetail(sf::RenderWindow &window, const std::vector<QuestState> &questStates);

    void drawText(sf::RenderWindow&, const std::string&, sf::Vector2f, int, sf::Color = sf::Color::White);
    void drawTextClamped(sf::RenderWindow&, std::string, sf::Vector2f, int, float maxWidth);

    void handleMouseClick(sf::Vector2f mousePos, const std::vector<QuestState> &questStates);

    bool isTracked(QuestID id) const;

public:
    QuestRenderer();

    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void update(float dt);

    void toggle();

    void setPosition(const sf::Vector2f &position);
    void setSize(const sf::Vector2f &size);

    void render(sf::RenderWindow &window, const std::vector<QuestState> &questStates);
};