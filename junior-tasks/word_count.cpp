// Implement word counting in a sentence
// `g++ -std=c++20 word_count.cpp`

#include <iostream>
#include <concepts>
#include <string>

namespace junior {

    template <typename CharT,
              typename Predicate>
     requires std::predicate<Predicate, CharT>
    std::size_t count_words(std::basic_string_view<CharT> text, Predicate is_delimiter) {
        if (text.empty()) { return 0; }

        std::size_t counter = 0;
        bool in_word = false;
        for (CharT chr : text) {
            if (is_delimiter(chr)) {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                ++counter;
            }
        }

        return counter;
    }

    template <typename CharT,
              typename Predicate>
     requires std::predicate<Predicate, CharT>
    std::size_t count_words(const std::basic_string<CharT>& text, Predicate is_delimiter) {
        return count_words(std::basic_string_view<CharT>(text), is_delimiter);
    }

}

int main() {
    std::string input;
    std::getline(std::cin, input);
    std::cout << junior::count_words(input, [](const auto chr) { return chr == ' '; });
    return 0;
}