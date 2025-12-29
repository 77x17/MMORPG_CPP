#pragma once

class GameWorld;
class NetworkServer;
class InterestSystem;

class GameWorldSyncSystem {
private:
    GameWorld      &gameWorld;
    NetworkServer  &networkServer;
    InterestSystem &interestSystem;

public:
    GameWorldSyncSystem(GameWorld &_gameWorld, NetworkServer &_networkServer, InterestSystem &_interestSystem);

    void syncToClients();
};