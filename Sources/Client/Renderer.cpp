#include "Renderer.hpp"

#include <SFML/Graphics.hpp>

Renderer::Renderer(sf::RenderWindow &_window) : window(_window) {
    if (!font.loadFromFile("Assets/Roboto_Mono.ttf")) {
        font.loadFromFile("../Assets/Roboto_Mono.ttf");
    }
}

void Renderer::render(const EntityManager &entityManager, int myId) {
    window.clear(sf::Color(30, 30, 30));

    sf::RectangleShape playerShape({ 40, 40 }); playerShape.setOrigin(20, 20);
    sf::CircleShape projectileShape(5); projectileShape.setOrigin(5, 5);

    for (auto &[id, player] : entityManager.getPlayers()) {
        playerShape.setPosition(player.localPosition);
        playerShape.setFillColor(player.id == myId ? sf::Color::Blue : sf::Color::Red);

        window.draw(playerShape);

        sf::Text nametag(std::to_string(player.id) + ". HP: " + std::to_string(player.hp), font, 12);
        sf::FloatRect bounds = nametag.getLocalBounds();
        nametag.setOrigin(bounds.left + bounds.width / 2.0f,
                          bounds.top  + bounds.height      );
        nametag.setPosition(player.localPosition - sf::Vector2f(0, 30));
        window.draw(nametag);
    }

    for (auto &[id, projectile] : entityManager.getProjectiles()) {
        projectileShape.setPosition(projectile.localPosition);
        projectileShape.setFillColor(projectile.ownerId == myId ? sf::Color::Yellow : sf::Color::White);

        window.draw(projectileShape);
    }

    for (auto &projectile : entityManager.getPredictedProjectiles()) {
        projectileShape.setPosition(projectile.localPosition);
        projectileShape.setFillColor(sf::Color::Yellow);

        window.draw(projectileShape);
    }

    window.display();
}