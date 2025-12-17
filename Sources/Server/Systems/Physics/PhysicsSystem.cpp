#include "Server/Systems/Physics/PhysicsSystem.hpp"

#include "Shared/Utils.hpp"
#include "Shared/Constants.hpp"
#include "Shared/InputState.hpp"

#include "Server/Systems/Physics/WorldCollision.hpp"
#include "Server/Entities/Player.hpp"

PhysicsSystem::PhysicsSystem(WorldCollision &_worldCollision) 
: worldCollision(_worldCollision) {

}

void PhysicsSystem::moveWithCollision(Player &player, const sf::Vector2f &delta) {
    sf::Vector2f proposedPosition = player.getPosition();

    sf::Vector2f proposedX = proposedPosition + sf::Vector2f(delta.x, 0.0f);
    if (not worldCollision.checkCollision(player.buildAABB(proposedX))) {
        proposedPosition.x = proposedX.x;
    }

    sf::Vector2f proposedY = proposedPosition + sf::Vector2f(0.0f, delta.y);
    if (not worldCollision.checkCollision(player.buildAABB(proposedY))) {
        proposedPosition.y = proposedY.y;
    }

    proposedPosition.x = std::clamp(proposedPosition.x, 0.0f, 3200.0f);
    proposedPosition.y = std::clamp(proposedPosition.y, 0.0f, 3200.0f);

    player.setPosition(proposedPosition);
}

void PhysicsSystem::updatePlayer(Player &player, const InputState &inputState, const float &dt) {
    sf::Vector2f inputVelocity{ 0.0f, 0.0f };

    if (inputState.movementDir.x != 0 || inputState.movementDir.y != 0) {
        inputVelocity = normalize(inputState.movementDir) * PLAYER_SPEED;
        player.setOldShootDir(inputState.movementDir);
    }

    sf::Vector2f velocity = inputVelocity + player.getImpulse();
    
    sf::Vector2f proposedPosition = player.getPosition();

    sf::Vector2f proposedX = proposedPosition + sf::Vector2f(velocity.x * dt, 0.0f);
    if (not worldCollision.checkCollision(player.buildAABB(proposedX))) {
        proposedPosition.x = proposedX.x;
    }
    else {
        velocity.x = 0.0f;
    }

    sf::Vector2f proposedY = proposedPosition + sf::Vector2f(0.0f, velocity.y * dt);
    if (not worldCollision.checkCollision(player.buildAABB(proposedY))) {
        proposedPosition.y = proposedY.y;
    }
    else {
        velocity.y = 0.0f;
    }
    
    proposedPosition.x = std::clamp(proposedPosition.x, 0.0f, 3200.0f);
    proposedPosition.y = std::clamp(proposedPosition.y, 0.0f, 3200.0f);

    player.setPosition(proposedPosition);
    player.setVelocity(inputVelocity);
}

void PhysicsSystem::resolvePlayerCollisions(const std::vector<Player *> &players) {
    for (size_t i = 0; i < players.size(); ++i) {
        for (size_t j = i + 1; j < players.size(); ++j) {
            if (not players[i]->getBounds().intersects(players[j]->getBounds())) {
                continue;
            }
            
            sf::FloatRect ra = players[i]->getBounds();
            sf::FloatRect rb = players[j]->getBounds();

            float overlapX = std::min(ra.left + ra.width, rb.left + rb.width)
                       - std::max(ra.left, rb.left);
            float overlapY = std::min(ra.top + ra.height, rb.top + rb.height)
                        - std::max(ra.top, rb.top);

            float penetration = std::min(overlapX, overlapY);
            if (penetration <= 0) continue;

            if (overlapX < overlapY) {
                sf::Vector2f push{ overlapX * 0.5f * (ra.left < rb.left ? -1.f : 1.f), 0.f };
                moveWithCollision(*players[i], push);
                moveWithCollision(*players[j], -push);
            }
            else {
                sf::Vector2f push{ 0.f, overlapY * 0.5f * (ra.top < rb.top ? -1.f : 1.f) };
                moveWithCollision(*players[i], push);
                moveWithCollision(*players[j], -push);
            }
        }
    }
}   