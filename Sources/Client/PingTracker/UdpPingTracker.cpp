#include "Client/PingTracker/UdpPingTracker.hpp"

#include <chrono>

uint64_t UdpPingTracker::nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

void UdpPingTracker::update(float dt) {
    accumulator += dt;
}

bool UdpPingTracker::shouldSendPing() const {
    return accumulator >= interval;
}

uint64_t UdpPingTracker::onPingSent() {
    accumulator = 0.0f;
    lastSendTimeMs = nowMs();
    return lastSendTimeMs;
}

void UdpPingTracker::onPingResponse(uint64_t clientSendTimeMs) {
    uint64_t now = nowMs();
    pingMs = static_cast<int>(now - clientSendTimeMs);

    if (smoothedPingMs == 0)
        smoothedPingMs = pingMs;
    else
        smoothedPingMs = static_cast<int>(smoothedPingMs * 0.8f + pingMs * 0.2f);
}

int UdpPingTracker::getPing() const {
    return smoothedPingMs;
}
