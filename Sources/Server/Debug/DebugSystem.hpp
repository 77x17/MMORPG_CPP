#pragma once

#include "Server/Debug/DebugSnapshot.hpp"

class GameWorld;
class NetworkServer;

class DebugSystem {
private:
    DebugSnapshot snapshot;

    bool playersDirty  = true;
    bool enemiesDirty  = true;
    bool entitiesDirty = true;
    bool logsDirty     = true;

private:
    void rebuildPlayers(const GameWorld &gameWorld, const NetworkServer &networkServer);
    void rebuildEnemies(const GameWorld &gameWorld);
    void rebuildEntities(const GameWorld &gameWorld);
    void rebuildLogs();

public:
    DebugSystem();

    void markPlayersDirty();
    void markEnemiesDirty();
    void markEntitiesDirty();
    void markLogsDirty();

    const DebugSnapshot & getSnapshot(const GameWorld &gameWorld, const NetworkServer &networkServer);
};