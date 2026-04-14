// Implement the vector class
// `g++ -std=c++20 vector.cpp`

#include <cstdint>
#include <cmath>
#include <iostream>
#include <limits>

namespace junior {

template <typename T>
class vector {
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    std::byte* arr_ = nullptr;

    T* data_() { return reinterpret_cast<T*>(arr_); }
    const T* data_() const { return reinterpret_cast<T*>(arr_); }

    void free_arr_(std::byte* arr, std::size_t size) {
       T* data = reinterpret_cast<T*>(arr);
       for (std::size_t i = 0; i < size; ++i) {
           data[i].~T();
       }
       ::delete[] arr;
    }

 public:
    vector() = default;
    ~vector() { free_arr_(arr_); }

    vector(std::size_t elements_count, T init_value = T()) {
        capacity_ = elements_count;
        size_ = elements_count;
        arr_ = ::new std::byte[capacity_ * sizeof(T)];
        T* data = data_();
        for (std::size_t i = 0; i < size_; ++i) {
            new (&data_()[i]) T(init_value);
        }
    }

    vector(const vector<T>& other) {
        T* other_data = other.data_();
        std::byte* arr_new = ::new std::byte[other.capacity_ * sizeof(T)];
        T* new_data = reinterpret_cast<T*>(arr_new);
        for (std::size_t i = 0; i < other.size_; ++i) {
            new (&new_data()[i]) T(other_data[i]);
        }

        std::byte* arr_old = arr_;
        arr_ = arr_new;
        size_ = other.size_;
        capacity_ = other.capacity_;
        free_arr_(arr_old);
    }

    vector(vector<T>&& other) {
        arr_ = other.arr_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.arr_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    vector<T>& operator=(const vector<T>& other) {
        if (this == &other) { return *this; }
        //
        return vector(other);
    }

    vector<T>& operator=(vector<T>&& other) {
        if (this == &other) { return *this; }
        //
        return vector(std::move(other));
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    void reserve(std::size_t elements_count) {
        if (capacity_ >= elements_count) { return; }
        T* data = data_();

        std::size_t new_capacity = std::max(elements_count, capacity_ * 2);
        std::byte* arr_new = ::new std::byte[new_capacity * sizeof(T)];
        T* new_data = reinterpret_cast<T*>(arr_new);
        for (std::size_t i = 0; i < size_; ++i) {
            new (&new_data[i]) T(data[i]);
        }

        std::byte* arr_old = arr_;
        arr_ = arr_new;
        capacity_ = new_capacity;
        free_arr_(arr_old);
    }

    T& operator[](std::size_t pos) { return data_()[pos]; }
    const T& operator[](std::size_t pos) const { return data_()[pos]; }

    void push_back(const T& elem) {
        reserve(size_ + 1);
        new (&data_()[size_]) T(elem);
        ++size_;
    }

    void pop_back() {
        if (size_ == 0) { return; }
        data_()[size_ - 1]->~T();
        --size_;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        reserve(size_ + 1);
        new (&data_()[size_]) T(std::forward<Args>(args)...);
        ++size_;
    }

    friend std::ostream& operator<<(std::ostream& out, const vector<T>& vec) {
        auto data = vec.data_();
        for (std::size_t i = 0; i < vec.size_; ++i) {
            out << data[i] << ' ';
        }
        return out;
    }
};

template <typename T>
struct Heavy {
    const T field = 0;
    
    Heavy() = default;
    Heavy(T field) : field(std::move(field)) {
        volatile int dummy = 0;
        while (dummy < 10000) { dummy = dummy + 1; }
    }
    
    friend std::ostream& operator<<(std::ostream& out, const Heavy& heavy) {
        out << heavy.field;
        return out;
    }
};

}

int main() {
    junior::vector<junior::Heavy<int>> vec(100);
    vec.reserve(100000);
    vec.emplace_back(5);
    std::cout << vec;
    return 0;
}