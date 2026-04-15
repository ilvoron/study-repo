// Write an implementation of the function `int atoi(const char *str)`, which converts a string into a number
// `g++ -std=c++20 atoi.cpp`

#include <stdexcept>
#include <iostream>
#include <limits>
#include <cctype>

namespace junior {

int atoi(const char* str) {
    if (!str) { return 0; }
    while (*str != '\0' && std::isspace(static_cast<unsigned char>(*str))) { ++str; }

    int sign = 1;
    if (*str != '\0') {
        if (*str == '-') {
            sign = -1;
            ++str;
        }
        else if (*str == '+') { ++str; }
    } else {
        return 0;
    }

    int result = 0;
    const int limit_digit = sign == 1 ? 7 : 8;
    while (*str != '\0') {
        if (*str < '0' || *str > '9') { break; }
        const int add = static_cast<int>(*str - '0');
        
        if (result > std::numeric_limits<int>::max() / 10 ||
            ((result == std::numeric_limits<int>::max() / 10) &&
             (add > limit_digit))) {
              if (sign == 1) { return std::numeric_limits<int>::max(); }
              return std::numeric_limits<int>::min();
        }

        result *= 10;
        result += add;
        ++str;
    }

    return sign == -1 ? -result : result;
}

}

int main() {
    std::string input;
    std::getline(std::cin, input);
    std::cout << junior::atoi(input.c_str());
    return 0;
}