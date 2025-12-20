#include "Random.hpp"

std::mt19937_64 Random::rng;
std::mutex Random::rngMutex;

void Random::seed(uint64_t seed) {
    std::lock_guard<std::mutex> lock(rngMutex);
    rng.seed(seed);
}

int Random::rangeInt(int min, int max) {
    std::lock_guard<std::mutex> lock(rngMutex);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

float Random::rangeFloat(float min, float max) {
    std::lock_guard<std::mutex> lock(rngMutex);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

bool Random::chance(float probability) {
    std::lock_guard<std::mutex> lock(rngMutex);
    std::bernoulli_distribution dist(probability);
    return dist(rng);
}
