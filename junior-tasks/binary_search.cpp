// Implement a binary search in an array
// `g++ -std=c++20 binary_search.cpp`

#include <concepts>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>

namespace junior {

    // Prerequirements: [start; end) is a valid range and sorted by Predicate.
    // Returns end if not found.
    template <typename T,
              typename It,
              typename Predicate = std::less<T>>
     requires (std::random_access_iterator<It> &&
               std::same_as<std::iter_value_t<It>, T> &&
               std::predicate<Predicate, T, T>)
    It binary_search(It start, It end, const T& value, Predicate comp = Predicate()) {
        It not_found = end;
        while (start < end) {
            auto middle = start + (std::distance(start, end) / 2);
            if (!comp(*middle, value) && !comp(value, *middle)) { return middle; }
            if (comp(*middle, value)) { start = middle + 1; }
            else { end = middle; }
        }
        return not_found;
    }

}

int main() {
    std::vector<int> vec(100);
    std::iota(vec.begin(), vec.end(), -10);
    std::copy(vec.cbegin(), vec.cend(), std::ostream_iterator<int>(std::cout, " "));
    int wanna_find;
    std::cout << '\n';
    std::cin >> wanna_find;
    auto it = junior::binary_search(vec.cbegin(), vec.cend(), wanna_find);
    if (it != vec.end()) {
        std::cout << "Found position: " << std::distance(vec.cbegin(), it);
    } else {
        std::cout << "Not Found";
    }
    return 0;
}