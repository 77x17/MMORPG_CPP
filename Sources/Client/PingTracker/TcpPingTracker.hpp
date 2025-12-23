#pragma once

#include <cstdint>

class TcpPingTracker {
private:
    uint64_t lastSendTimeMs = 0;
    int pingMs = 0;
    int smoothedPingMs = 0;

    float interval    = 1.0f;      
    float accumulator = 0.0f;

public:
    static uint64_t nowMs();

    void update(float dt);
    bool shouldSendPing() const;

    void onPingSent();
    void onPingResponse(uint64_t clientSendTimeMs);

    int getPing() const;
};
