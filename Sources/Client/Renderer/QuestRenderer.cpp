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

    buttonTrack.setSize({120.f, 28.f});
    buttonTrack.setFillColor(sf::Color(60, 60, 60));
    buttonTrack.setOutlineThickness(1.0f);
    buttonTrack.setOutlineColor(sf::Color::White);
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
    // drawBackground(window);
    // drawQuests(window, questStates);

    if (uiState == QuestUIState::TrackingOnly) {
        drawTrackingPanel(window, questStates);
    }
    else {
        drawFullWindow(window, questStates);
    }
}

void QuestRenderer::toggle() {
    uiState = (uiState == QuestUIState::TrackingOnly) ? QuestUIState::FullWindow : QuestUIState::TrackingOnly;
}

void QuestRenderer::update(float dt) {}

void QuestRenderer::handleEvent(const sf::Event &event, const sf::RenderWindow &window) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Q) {
        toggle();
    }

    if (uiState != QuestUIState::FullWindow)
        return;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos =
            window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        // click xử lý trong render (pass quests)
        (void)mousePos;
    }
}

void QuestRenderer::drawTrackingPanel(sf::RenderWindow& window, const std::vector<QuestState>& quests) {
    background.setSize({260.f, 160.f});
    background.setPosition({
        window.getSize().x - 270.f,
        20.f
    });

    window.draw(background);

    float y = background.getPosition().y + 10.f;

    drawText(window, "Quest Tracker",
        {background.getPosition().x + 10, y}, 14, sf::Color::Yellow);

    y += 20;

    for (const auto& quest : quests) {
        // if (!isTracked(quest.questId)) continue;
        if (quest.status != QuestStatus::InProgress) continue;

        drawTextClamped(
            window,
            std::to_string(quest.questId),
            {background.getPosition().x + 10, y},
            13,
            background.getSize().x - 20
        );
        y += 16;

        for (const auto& obj : quest.objectives) {
            if (y > background.getPosition().y + background.getSize().y - 18)
                return;

            drawTextClamped(
                window,
                std::format("- {} {}/{}", "description", obj.current, "target"),
                {background.getPosition().x + 20, y},
                11,
                background.getSize().x - 30
            );
            y += 14;
        }
        y += 6;
    }
}

void QuestRenderer::drawFullWindow(sf::RenderWindow& window, const std::vector<QuestState>& quests) {
    background.setSize({720.f, 460.f});
    background.setPosition({80.f, 60.f});
    window.draw(background);

    drawQuestList(window, quests);
    drawQuestDetail(window, quests);
}

void QuestRenderer::drawQuestList(sf::RenderWindow& window, const std::vector<QuestState>& quests) {
    float x = background.getPosition().x + 20;
    float y = background.getPosition().y + 20;

    drawText(window, "Quest Log", {x, y}, 18, sf::Color::Yellow);
    y += 30;

    for (const auto& quest : quests) {
        sf::Color color =
            (quest.questId == selectedQuest)
                ? sf::Color::Yellow
                : sf::Color::White;

        drawText(window, std::to_string(quest.questId), {x, y}, 14, color);
        y += 22;
    }
}

void QuestRenderer::drawQuestDetail(sf::RenderWindow& window, const std::vector<QuestState>& quests) {
    const QuestState* q = nullptr;
    for (const auto& quest : quests)
        if (quest.questId == selectedQuest)
            q = &quest;

    if (!q) return;

    float x = background.getPosition().x + 300;
    float y = background.getPosition().y + 20;

    drawText(window, std::to_string(q->questId), {x, y}, 18, sf::Color::Yellow);
    y += 30;

    drawTextClamped(window, std::to_string(q->questId), {x, y}, 13, 360.f);
    y += 60;

    for (const auto& obj : q->objectives) {
        drawText(window,
            std::format("- {} {}/{}", "description", obj.current, "target"),
            {x, y}, 12);
        y += 18;
    }

    // Track button
    buttonTrack.setPosition({x, background.getPosition().y + background.getSize().y - 50});
    window.draw(buttonTrack);

    drawText(
        window,
        isTracked(q->questId) ? "Untrack" : "Track",
        buttonTrack.getPosition() + sf::Vector2f(20, 5),
        13
    );
}

void QuestRenderer::drawText(sf::RenderWindow& window, const std::string& str, sf::Vector2f pos, int size, sf::Color color) {
    sf::Text text(str, Font::getFont(), size);
    text.setFillColor(color);
    text.setPosition(pos);
    window.draw(text);
}

void QuestRenderer::drawTextClamped(sf::RenderWindow& window, std::string text, sf::Vector2f pos, int size, float maxWidth) {
    sf::Text t(text, Font::getFont(), size);
    while (t.getLocalBounds().width > maxWidth && !text.empty()) {
        text.pop_back();
        t.setString(text + "...");
    }
    t.setPosition(pos);
    window.draw(t);
}

bool QuestRenderer::isTracked(QuestID id) const {
    return trackedQuests.contains(id);
}
