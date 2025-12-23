#pragma once

#include <cstdint>

class UdpPingTracker {
private:
    uint64_t lastSendTimeMs = 0;
    int pingMs = 0;
    int smoothedPingMs = 0;

    float interval    = 0.5f;
    float accumulator = 0.0f;

public:
    static uint64_t nowMs();

    void update(float dt);
    bool shouldSendPing() const;

    uint64_t onPingSent();
    void onPingResponse(uint64_t clientSendTimeMs);

    int getPing() const;
};
