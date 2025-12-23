#pragma once

#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    std::mutex mtx;

public:
    void push(const T& v) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(v);
    }
    
    bool tryPop(T& out){
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty()) { return false; }
        out = q.front();
        q.pop();
        return true;
    }

    void drain(std::vector<T> &out) {
        std::lock_guard<std::mutex> lock(mtx);
        while (!q.empty()) {
            out.push_back(q.front());
            q.pop();
        }
    }
};