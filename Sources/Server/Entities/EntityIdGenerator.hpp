#pragma once

#include <atomic>

class EntityIdGenerator {
private:
    std::atomic<int> currentId;

public:
    EntityIdGenerator(int start = 1) : currentId(start) {}

    int next() {
        return currentId.fetch_add(1, std::memory_order_relaxed);
    }
};