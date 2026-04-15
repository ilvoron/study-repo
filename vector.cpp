// Implement the vector class
// `g++ -std=c++20 vector.cpp`

#include <cstdint>
#include <cmath>
#include <iostream>
#include <limits>
#include <utility>
#include <algorithm>

namespace junior {

template <typename T>
class vector {
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    T* arr_ = nullptr;

    void free_arr_(T* arr, std::size_t size) {
        if (!arr) { return; }
        for (std::size_t i = 0; i < size; ++i) {
            arr[i].~T();
        }
        ::operator delete(arr);
    }

    void swap(vector& other) noexcept {
        std::swap(arr_, other.arr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

 public:
    vector() = default;
    ~vector() { free_arr_(arr_, size_); }

    vector(std::size_t elements_count)
     requires (std::is_default_constructible_v<T>) :
     capacity_(elements_count), size_(elements_count) {
        arr_ = static_cast<T*>(::operator new (capacity_ * sizeof(T)));
        for (std::size_t i = 0; i < size_; ++i) {
            ::new (&arr_[i]) T();
        }
    }

    vector(std::size_t elements_count, const T& init_value)
     requires (std::is_copy_constructible_v<T>) :
     capacity_(elements_count), size_(elements_count) {
        arr_ = static_cast<T*>(::operator new (capacity_ * sizeof(T)));
        for (std::size_t i = 0; i < size_; ++i) {
            ::new (&arr_[i]) T(init_value);
        }
    }

    vector(const vector<T>& other) {
        vector<T> copy;
        copy.capacity_ = other.capacity_;
        copy.arr_ = static_cast<T*>(::operator new(copy.capacity_ * sizeof(T)));
        for (std::size_t i = 0; i < other.size_; ++i) {
            copy.push_back(other.arr_[i]);
        }
        swap(copy);
    }

    vector(vector<T>&& other) noexcept { swap(other); }

    vector<T>& operator=(const vector<T>& other) {
        if (this == &other) { return *this; }
        vector<T> copy(other);
        swap(copy);
        return *this;
    }

    vector<T>& operator=(vector<T>&& other) noexcept {
        if (this == &other) { return *this; }
        swap(other);
        return *this;
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    void reserve(std::size_t elements_count) {
        if (capacity_ >= elements_count) { return; }

        std::size_t new_capacity = std::max(elements_count, capacity_ * 2);
        vector<T> copy;
        copy.capacity_ = new_capacity;
        copy.arr_ = static_cast<T*>(::operator new(copy.capacity_ * sizeof(T)));
        for (std::size_t i = 0; i < size_; ++i) {
            copy.push_back(std::move_if_noexcept(arr_[i]));
        }

        swap(copy);
    }

    void resize(std::size_t elements_count)
     requires (std::is_default_constructible_v<T>) {
        reserve(elements_count);
        while (elements_count < size_) { pop_back(); }
        while (elements_count > size_) { emplace_back(); }
    }

    void resize(std::size_t elements_count, const T& init_value)
     requires (std::is_copy_constructible_v<T>) {
        reserve(elements_count);
        while (elements_count < size_) { pop_back(); }
        while (elements_count > size_) { push_back(init_value); }
    }

    T& operator[](std::size_t pos) { return arr_[pos]; }
    const T& operator[](std::size_t pos) const { return arr_[pos]; }

    void push_back(const T& elem) { emplace_back(elem); }
    void push_back(T&& elem) { emplace_back(std::move(elem)); }

    void pop_back() {
        if (size_ == 0) { return; }
        arr_[size_ - 1].~T();
        --size_;
    }

    // UB if uses smth like `vec.emplace_back(vec[0])` that causes reallocate
    template <typename... Args>
    void emplace_back(Args&&... args) {
        reserve(size_ + 1);
        ::new (&arr_[size_]) T(std::forward<Args>(args)...);
        ++size_;
    }

    friend std::ostream& operator<<(std::ostream& out, const vector<T>& vec) {
        for (std::size_t i = 0; i < vec.size_; ++i) {
            out << vec.arr_[i] << ' ';
        }
        return out;
    }
};

template <typename T>
struct Heavy {
    inline static T field = 0;

    Heavy() {
        volatile int dummy = 0;
        while (dummy < 10000) { dummy = dummy + 1; }
        ++field;
    }

    friend std::ostream& operator<<(std::ostream& out, const Heavy& heavy) {
        out << heavy.field;
        return out;
    }
};

}

int main() {
    junior::vector<junior::Heavy<int>> vec;
    vec.reserve(10000);
    std::cout << vec << std::endl;
    vec.emplace_back();
    std::cout << vec << std::endl;
    vec.resize(10);
    std::cout << vec << std::endl;
    return 0;
}