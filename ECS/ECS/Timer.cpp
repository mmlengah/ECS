#include "Timer.h"

Timer::Timer() {
    previousTime = std::chrono::high_resolution_clock::now();
}

float Timer::GetDeltaTime() {
    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - previousTime);
    previousTime = currentTime;
    return deltaTime.count();
}