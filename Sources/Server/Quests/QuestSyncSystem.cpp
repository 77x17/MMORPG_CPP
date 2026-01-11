#include "Server/Quests/QuestSyncSystem.hpp"

#include "Server/Network/NetworkServer.hpp"

#include "Server/Quests/PlayerQuestState.hpp"
#include "Server/Quests/QuestPrinter.hpp"

#include "Shared/TcpPacketType.hpp"

void QuestSyncSystem::syncToClient(const std::vector<PlayerQuestState> &questStates, int clientId, NetworkServer &networkServer) {
    sf::Packet packet; packet << static_cast<uint8_t>(TcpPacketType::QuestFullSync);
    packet << questStates.size();
    for (const PlayerQuestState &questState : questStates) {
        packet << questState.questId;
        packet << static_cast<uint8_t>(questState.status);
        packet << questState.objectives.size();
        for (const ObjectiveState &objectiveState : questState.objectives) {
            packet << static_cast<uint8_t>(objectiveState.type);
            packet << objectiveState.current;
            packet << objectiveState.completed;
        }
    }
    networkServer.sendAsync(clientId, packet, false);
}