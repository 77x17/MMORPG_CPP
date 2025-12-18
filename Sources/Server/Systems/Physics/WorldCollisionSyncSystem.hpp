#pragma once

class WorldCollision;
class NetworkServer;

class WorldCollisionSyncSystem {
private:
    WorldCollision &worldCollision;
    NetworkServer  &networkServer;

public:
    WorldCollisionSyncSystem(WorldCollision &_worldCollision, NetworkServer &_networkServer);

    void syncToClient(int clientId);
};