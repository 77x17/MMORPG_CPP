#pragma once

class GameWorld;
class NetworkServer;

class DebugChunkSyncSystem {
private:
    GameWorld     &gameWorld;
    NetworkServer &networkServer;

public:
    DebugChunkSyncSystem(GameWorld &_gameWorld, NetworkServer &_networkServer);

    void syncToClients();
};