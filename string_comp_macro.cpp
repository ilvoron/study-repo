// Implement a macro to compare two strings
// `g++ -std=c++20 string_comp_macro.cpp`

#include <iostream>

#define IS_STR_EQUALS(str1, str2) \
    ([&]() -> bool { \
        if (!str1 || !str2) { return false; } \
        const char* str1_ = str1; \
        const char* str2_ = str2; \
        while (*str1_ != 0 && *str2_ !=0) { \
            if (*str1_ != *str2_) { return false; } \
            ++str1_; \
            ++str2_; \
        } \
        if (*str1_ == 0 && *str2_ == 0) { return true; } \
        return false; \
    }())

#define IS_STR_EQUALS_MODERN(str1, str2) \
    (str1 && str2 && std::string_view(str1) == std::string_view(str2))

namespace junior {

    constexpr bool is_str_equals(std::string_view str1, std::string_view str2) {
        return str1 == str2;
    }

}

int main() {
    const char* str1 = "hello";
    const char* str2 = "hello";
    const char* str3 = "hello123";
    std::cout << std::boolalpha;
    std::cout << IS_STR_EQUALS(str2, str3) << '\n';
    std::cout << IS_STR_EQUALS(str1, str2) << '\n';
    std::cout << IS_STR_EQUALS_MODERN(str2, str3) << '\n';
    std::cout << IS_STR_EQUALS_MODERN(str1, str2) << '\n';
    std::cout << junior::is_str_equals(str2, str3) << '\n';
    std::cout << junior::is_str_equals(str1, str2) << '\n';
}