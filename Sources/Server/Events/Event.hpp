#pragma once

struct Event {
    virtual ~Event() = default;
};

struct PlayerSpawnedEvent : Event {
    int playerId;
    PlayerSpawnedEvent(int id) : playerId(id) {}
};

struct PlayerRemovedEvent : Event {
    int playerId;
    PlayerRemovedEvent(int id) : playerId(id) {}
};

struct EnemySpawnedEvent : Event {
    int enemyId;
    EnemySpawnedEvent(int id) : enemyId(id) {}
};

struct EnemyRemovedEvent : Event {
    int enemyId;
    EnemyRemovedEvent(int id) : enemyId(id) {}
};