#include "Client/Renderer/QuestRenderer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "Client/Utils/Font.hpp"

#include "Client/Quests/QuestStates.hpp"

#include <sstream>
#include <format>

void QuestRenderer::drawBackground(sf::RenderWindow &window) {
    window.draw(background);
}

void QuestRenderer::drawQuests(sf::RenderWindow &window, const std::vector<QuestState> &questStates) {
    sf::Text label; 
    label.setFont(Font::getFont());
    label.setCharacterSize(13);
    label.setString("Quests");

    sf::FloatRect bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
    label.setPosition({ questPosition.x + questSize.x / 2.0f, questPosition.y + 5.0f });

    window.draw(label);

    std::ostringstream oss;
    for (const QuestState &questState : questStates) {
        oss << std::format(
            "[{}][{}]\n",
            questState.questId,
            questState.status == QuestStatus::InProgress ? "In Progress" : "Completed"
        );
        for (const ObjectiveState &objectiveState : questState.objectives) {
            oss << std::format(
                "Type: {} | Current: {} | Status: {}\n",
                objectiveState.type == ObjectiveType::Kill ? "Kill" : "Reach",
                objectiveState.current,
                objectiveState.completed
            );
        }
    }

    sf::Text quests; 
    quests.setFont(Font::getFont());
    quests.setCharacterSize(10);
    quests.setString(oss.str());
    quests.setPosition({ questPosition.x, questPosition.y + 15.0f });

    window.draw(quests);
}

QuestRenderer::QuestRenderer() {
    background.setFillColor(sf::Color(20, 20, 20, 180));
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(sf::Color::White);
}

void QuestRenderer::setPosition(const sf::Vector2f &position) {
    questPosition = position;
    background.setPosition(questPosition);
}

void QuestRenderer::setSize(const sf::Vector2f &size) {
    questSize = size;
    background.setSize(questSize);
}

void QuestRenderer::render(sf::RenderWindow &window, const std::vector<QuestState> &questStates) {
    drawBackground(window);
    drawQuests(window, questStates);
}
