#pragma once

#include <vector>

class PlayerQuestState;
class NetworkServer;

class QuestSyncSystem {
public:
    static void syncToClient(const std::vector<PlayerQuestState> &questStates, int clientId, NetworkServer &networkServer);
};