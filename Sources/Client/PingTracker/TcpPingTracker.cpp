#include "Client/PingTracker/TcpPingTracker.hpp"

#include <chrono>

uint64_t TcpPingTracker::nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

void TcpPingTracker::update(float dt) {
    accumulator += dt;
}

bool TcpPingTracker::shouldSendPing() const {
    return accumulator >= interval;
}

void TcpPingTracker::onPingSent() {
    accumulator = 0.0f;
    lastSendTimeMs = nowMs();
}

void TcpPingTracker::onPingResponse(uint64_t clientSendTimeMs) {
    uint64_t now = nowMs();
    pingMs = static_cast<int>(now - clientSendTimeMs);

    if (smoothedPingMs == 0)
        smoothedPingMs = pingMs;
    else
        smoothedPingMs = static_cast<int>(smoothedPingMs * 0.8f + pingMs * 0.2f);
}

int TcpPingTracker::getPing() const {
    return smoothedPingMs;
}
