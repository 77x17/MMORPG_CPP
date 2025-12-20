#pragma once
#include <random>
#include <mutex>

class Random {
private:
    static std::mt19937_64 rng;
    static std::mutex rngMutex;

public:
    static void seed(uint64_t seed);

    static int rangeInt(int min, int max);
    static float rangeFloat(float min, float max);

    static bool chance(float probability); // 0.0 → 1.0
};
