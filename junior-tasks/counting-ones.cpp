// Count the number of ones in any given number
// `g++ -std=c++20 counting-ones.cpp`

#include <iostream>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <bit>

template <std::integral T>
requires (!std::same_as<T, bool>)
std::size_t count_ones(T num) {
    std::size_t counter = 0;

    while (num != 0) {
        if (num % 10 == 1 || num % 10 == -1) {
            ++counter;
        }
        num /= 10;
    }

    return counter;
}

int main() {
    int num1 = 124124141;
    long long num2 = -48239081111324121;
    unsigned int num3 = 43121112;

    std::cout << count_ones(num1) << '\n'; // 4
    std::cout << count_ones(num2) << '\n'; // 6
    std::cout << count_ones(num3) << std::endl; // 4
    
    std::uint8_t num4 = 0;
    std::uint8_t num5 = 0b00011101;
    std::uint8_t num6 = 0b11111111;

    std::cout << std::popcount(num4) << '\n'; // 0
    std::cout << std::popcount(num5) << '\n'; // 4
    std::cout << std::popcount(num6) << std::endl; // 8

    return 0;
}