// Implement the vector class
// `g++ -std=c++20 vector.cpp`

#include <cstdint>
#include <cmath>
#include <limits>

namespace junior {

template <typename T>
class vector {
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    std::byte* arr_ = nullptr;

 public:
    vector() = default;
 
    ~vector() {
       T* data = reinterpret_cast<T*>(arr_);
       for (std::size_t i = 0; i < size_; ++i) {
           data[i].~T();
       }
       ::delete[] arr_;
    }

    vector(std::size_t elements_count, T init_value = T()) {
        capacity_ = elements_count;
        size_ = elements_count;
        arr_ = new std::byte[capacity_ * sizeof(T)];
        for (std::size_t i = 0; i < size_; ++i) {
            new (arr_ + i * sizeof(T)) T(init_value);
        }
    }
    
    vector(const vector<T>& other) {
        T* other_data = reinterpret_cast<T*>(other.arr_);
        std::byte* arr_new = ::new std::byte[other.capacity_ * sizeof(T)];
        for (std::size_t i = 0; i < other.size_; ++i) {
            new (arr_new + i * sizeof(T)) T(other_data[i]);
        }
        
        std::byte* arr_old = arr_;
        arr_ = arr_new;
        size_ = other.size_;
        capacity_ = other.capacity_;
        ::delete[] arr_old;
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
        T* other_data = reinterpret_cast<T*>(other.arr_);
    
        std::byte* arr_new = ::new std::byte[other.capacity_ * sizeof(T)];
        for (std::size_t i = 0; i < other.size_; ++i) {
            new (arr_new + i * sizeof(T)) T(other_data[i]);
        }
        
        std::byte* arr_old = arr_;
        arr_ = arr_new;
        size_ = other.size_;
        capacity_ = other.capacity_;
        ::delete[] arr_old;
        return *this;
    }
    
    vector<T>& operator=(vector<T>&& other) {
        if (this == &other) { return *this; }
        arr_ = other.arr_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.arr_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }
    
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    void reserve(std::size_t elements_count) {
        if (capacity_ >= elements_count) { return; }
        T* data = reinterpret_cast<T*>(arr_);

        std::size_t new_capacity = std::max(elements_count, capacity_ * 2);
        std::byte* arr_new = ::new std::byte[new_capacity * sizeof(T)];
        for (std::size_t i = 0; i < size_; ++i) {
            data[i].~T();
            new (arr_new + i * sizeof(T)) T(data[i]);
        }
        
        std::byte* arr_old = arr_;
        arr_ = arr_new;
        capacity_ = new_capacity;
        ::delete[] arr_old;
    }
    
    T& operator[](std::size_t pos) { return reinterpret_cast<T*>(arr_)[pos]; }
    
    void push_back(const T& elem) {
        if (size_ >= capacity_) { reserve(size_ + 1); }
        new (arr_ + size_ * sizeof(T)) T(elem);
        ++size_;
    }
    
    void pop_back() {
        if (size_ == 0) { return; }
        reinterpret_cast<T*>(arr_)[size_ - 1]->~T();
        --size_;
    }
};

}

int main() {
    junior::vector<int> vec(200, 5);
    return 0;
}