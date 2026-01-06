#include "Server/Core/Chunk/DebugChunkSyncSystem.hpp"

#include "Server/Core/GameWorld.hpp"
#include "Server/Network/NetworkServer.hpp"

#include "Server/Entities/Player.hpp"

#include "Shared/TcpPacketType.hpp"

DebugChunkSyncSystem::DebugChunkSyncSystem(GameWorld &_gameWorld, NetworkServer &_networkServer) 
: gameWorld(_gameWorld), networkServer(_networkServer) {

}
    
void DebugChunkSyncSystem::syncToClients() {
    for (ClientSession &client : networkServer.getClients()) {
        Player *player = gameWorld.getPlayer(client.id);
        if (player == nullptr) continue;
        
        std::vector<ChunkCoord> chunks = gameWorld.getChunkInRange(player->getId(), player->getPosition());
        if (chunks.size() == 0) continue;
        
        sf::Packet chunkPacket;
        chunkPacket << static_cast<uint8_t>(TcpPacketType::Chunk) << (int)chunks.size();  
        for (const ChunkCoord &chunk : chunks) {
            chunkPacket << chunk.getX() * CHUNK_SIZE
                        << chunk.getY() * CHUNK_SIZE
                        << CHUNK_SIZE
                        << CHUNK_SIZE;
        }
        networkServer.sendAsync(client.id, chunkPacket, false);
    }
}