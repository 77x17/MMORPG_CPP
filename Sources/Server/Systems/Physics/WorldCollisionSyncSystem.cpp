#include "Server/Systems/Physics/WorldCollisionSyncSystem.hpp"

#include "Server/Systems/Physics/WorldCollision.hpp"
#include "Server/Network/NetworkServer.hpp"

WorldCollisionSyncSystem::WorldCollisionSyncSystem(WorldCollision &_worldCollision, NetworkServer &_networkServer) 
: worldCollision(_worldCollision), networkServer(_networkServer) {

}

// === This function need to be merge with InterestSystem, ChunkSystem, ... ===
//     Or WorldCollision must be in GameWorld ?
void WorldCollisionSyncSystem::syncToClient(int clientId) {
    sf::Packet worldCollisionPacket;
    worldCollisionPacket << "WorldCollision";

    const std::vector<AABB> &colliders = worldCollision.getColliders();
    worldCollisionPacket << static_cast<int>(colliders.size());
    for (const AABB &box : colliders) {
        worldCollisionPacket << box.position.x << box.position.y << box.size.x << box.size.y;
    }

    networkServer.sendToClientTcp(clientId, worldCollisionPacket);
}