#pragma once
#include <chrono>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point previousTime;

public:
    Timer();
    float GetDeltaTime();
};

