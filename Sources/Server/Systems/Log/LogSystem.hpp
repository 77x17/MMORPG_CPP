#pragma once

#include <deque>
#include <string>

class LogSystem {
private:
    static std::deque<std::string> logs;

public:
    static void addMessage(const std::string &message);

    static const std::deque<std::string> getLogs();
    static void clear();
};