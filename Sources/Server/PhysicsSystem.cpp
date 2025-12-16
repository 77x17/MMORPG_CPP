#include "PhysicsSystem.hpp"

#include "../Shared/Utils.hpp"
#include "../Shared/Constants.hpp"
#include "../Shared/InputState.hpp"

#include "WorldCollision.hpp"
#include "Player.hpp"

PhysicsSystem::PhysicsSystem(WorldCollision &_worldCollision) 
: worldCollision(_worldCollision) {

}

void PhysicsSystem::updatePlayer(Player &player, const InputState &inputState, const float &dt) {
    sf::Vector2f velocity{ 0.0f, 0.0f };

    if (inputState.movementDir.x != 0 || inputState.movementDir.y != 0) {
        velocity = normalize(inputState.movementDir) * PLAYER_SPEED;
        player.setOldShootDir(inputState.movementDir);
    }

    sf::Vector2f proposedPosition = player.getPosition() + player.getVelocity() * dt;
    
    proposedPosition.x = std::clamp(proposedPosition.x, 0.0f, 3200.0f);
    proposedPosition.y = std::clamp(proposedPosition.y, 0.0f, 3200.0f);
    
    AABB proposedBox = player.buildAABB(proposedPosition);

    if (not worldCollision.checkCollision(proposedBox)) {
        player.setPosition(proposedPosition);
        player.setVelocity(velocity);
    }
    else {
        player.setVelocity({ 0.0f, 0.0f });   
    }
}

void PhysicsSystem::resolvePlayerCollisions(const std::vector<Player *> &players) {
    for (size_t i = 0; i < players.size(); ++i) {
        for (size_t j = i + 1; j < players.size(); ++j) {
            if (not players[i]->getBounds().intersects(players[j]->getBounds())) {
                continue;
            }
            
            sf::Vector2f direction = normalize(players[j]->getPosition() - players[i]->getPosition());
            if (direction.x == 0 && direction.y == 0) {
                direction = { 1.0f, 0.0f };
            }
            
            sf::FloatRect ra = players[i]->getBounds();
            sf::FloatRect rb = players[j]->getBounds();

            float overlapX = std::min(ra.left + ra.width, rb.left + rb.width)
                       - std::max(ra.left, rb.left);
            float overlapY = std::min(ra.top + ra.height, rb.top + rb.height)
                        - std::max(ra.top, rb.top);

            float penetration = std::min(overlapX, overlapY);
            if (penetration <= 0) continue;

            sf::Vector2f push = direction * (penetration * 0.5f);
            players[i]->move(-push);
            players[j]->move( push);
        }
    }
}   