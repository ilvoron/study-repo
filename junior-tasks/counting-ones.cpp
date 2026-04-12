// Count the number of ones in any given number
// `g++ -std=c++20 counting-ones.cpp`

#include <iostream>
#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <cstdint>
#include <bit>

template <std::integral T, std::predicate<std::uint8_t> F>
requires (!std::same_as<T, bool>)
std::size_t count_digits_if(T num, F predicate) {
    std::size_t counter = 0;
    do {
        const auto rem = num % 10;
        const auto normalized_rem = rem >= 0 ? rem : -rem;
        if (predicate(static_cast<std::uint8_t>(normalized_rem))) {
            ++counter;
        }
        num /= 10;
    } while (num != 0);
    return counter;
}

template <std::integral T>
requires (!std::same_as<T, bool>)
std::size_t count_digits(T num, const std::uint8_t digit) {
    if (digit > 9) {
        throw std::invalid_argument("digit must be in [0, 9]");
    }

    return count_digits_if(num, [digit](std::uint8_t cur_digit) {
        return digit == cur_digit;
    });
}

template <std::integral T>
requires (!std::same_as<T, bool>)
std::size_t count_ones(T num) {
    return count_digits(num, 1);
}

int main() {
    int num1 = 124124141;
    long long num2 = -48239081111324121;
    unsigned int num3 = 43121112;

    std::cout << count_ones(num1) << '\n'; // 4
    std::cout << count_ones(num2) << '\n'; // 6
    std::cout << count_ones(num3) << '\n'; // 4
    
    std::cout << count_digits(num1, 2) << '\n'; // 2
    std::cout << count_digits(num2, 3) << '\n'; // 2
    std::cout << count_digits(num3, 1) << '\n'; // 4
    
    std::uint8_t num4 = 0;
    std::uint8_t num5 = 0b00011101;
    std::uint8_t num6 = 0b11111111;

    std::cout << std::popcount(num4) << '\n'; // 0
    std::cout << std::popcount(num5) << '\n'; // 4
    std::cout << std::popcount(num6) << std::endl; // 8

    return 0;
}