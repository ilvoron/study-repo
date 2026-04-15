// Reverse the string
// `g++ -std=c++20 reverse_string.cpp`

#include <string>
#include <algorithm>
#include <iostream>

namespace junior {

    std::string reverse(const std::string& original) {
        std::string reversed(original.crbegin(), original.crend());
        return reversed;
    }
    
    void reverse_in(std::string& original) {
        std::ranges::reverse(original);
    }

}

int main() {
    std::string input;
    std::cin >> input;
    std::cout << junior::reverse(input) << '\n';
    junior::reverse_in(input);
    std::cout << input << '\n';
}