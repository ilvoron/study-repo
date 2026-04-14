// Implement any sorting algorithm
// `g++ -std=c++20 quick_sort.cpp`

#include <random>
#include <vector>
#include <numeric>
#include <iostream>
#include <concepts>
#include <iterator>
#include <algorithm>

namespace junior {

    // Prerequirements: [start; end) is a valid range
    template <typename It,
              typename T = std::iter_value_t<It>,
              typename Predicate = std::less<T>>
     requires (std::forward_iterator<It> &&
               std::same_as<std::iter_value_t<It>, T> &&
               std::predicate<Predicate, T, T>)
    void quick_sort(It start, It end, Predicate comp = Predicate()) {
        if (std::distance(start, end) < 2) { return; }
        auto& pivot = *std::next(start, std::distance(start, end) / 2);

        auto middle1 = std::partition(start, end, [&](const auto& el) {
            return comp(el, pivot);
        });
        auto middle2 = std::partition(middle1, end, [&](const auto& el) {
            return !comp(pivot, el);
        });

        quick_sort(start, middle1, comp);
        quick_sort(middle2, end, comp);
    }

}

namespace junior::ranges {

    template <typename R,
              typename T = std::ranges::range_value_t<R>,
              typename Predicate = std::less<T>>
     requires (std::ranges::forward_range<R> &&
               std::predicate<Predicate, T, T>)
    void quick_sort(R& range, Predicate comp = Predicate()) {
        junior::quick_sort(std::ranges::begin(range), std::ranges::end(range), comp);
    }

}

int main() {
    std::random_device dev;
    std::mt19937 gen(dev());
    std::vector<int> vec(100);

    std::iota(vec.begin(), vec.end(), 0);
    std::ranges::copy(vec, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    std::ranges::shuffle(vec, gen);
    std::ranges::copy(vec, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    junior::quick_sort(vec.begin(), vec.end());
    std::ranges::copy(vec, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n' << '\n';

    std::iota(vec.begin(), vec.end(), 0);
    std::ranges::copy(vec, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    std::ranges::shuffle(vec, gen);
    std::ranges::copy(vec, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    junior::ranges::quick_sort(vec);
    std::ranges::copy(vec, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n' << '\n';

    return 0;
}