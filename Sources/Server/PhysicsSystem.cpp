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

    player.setVelocity(velocity);

    sf::Vector2f proposedPosition = player.getPosition() + player.getVelocity() * dt;

    // AABB proposedBox = player.buildAABB(proposedPosition);

    // if (not worldCollision.checkCollision(proposedBox)) {
    //     player.setPosition(proposedPosition);
    // }

    proposedPosition.x = std::clamp(proposedPosition.x, 0.0f, 3200.0f);
    proposedPosition.y = std::clamp(proposedPosition.y, 0.0f, 3200.0f);

    player.setPosition(proposedPosition);
}