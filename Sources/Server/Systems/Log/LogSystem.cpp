#include "Server/Systems/Log/LogSystem.hpp"

#include <chrono>
#include <format>

using namespace std::chrono;

void LogSystem::addMessage(const std::string &message) {
    auto now = system_clock::now();
    auto localTime = floor<seconds>(now);

    std::string timeString = std::format(
        "[{:%H:%M:%S}]",
        localTime
    );

    if (logs.size() >= 1024) {
        logs.pop_front();
    }

    logs.push_back(timeString + message);
}

const std::deque<std::string> LogSystem::getLogs() {
    return logs;
}

void LogSystem::clear() {
    logs.clear();
}