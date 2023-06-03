//PCR.h stands for Practial Components Random
#pragma once
#include <random>
#include <type_traits>

namespace PC {
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, T>::type
        randomRange(T min, T max) {
        // Set up a random number generator
        std::random_device rd;
        std::default_random_engine gen(rd());

        // Define the range of the random number
        std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>> dis(min, max);

        // Generate a random number within the range
        T random_num = dis(gen);
        return random_num;
    }
}