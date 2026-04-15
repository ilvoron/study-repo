// Implement a shared_ptr with an extension for weak_ptr
// `g++ -std=c++20 shared_ptr.cpp`

#include <concepts>
#include <utility>
#include <iostream>
#include <atomic>

namespace senior {

    template <typename T>
    struct __control_block {
        T* value{nullptr};
        std::atomic<int> strong_ref_count{1};

        ~__control_block() {
            if (value) { delete value; }
        }
    };

    template <typename T>
    class shared_ptr final {
        __control_block<T>* control_block_{nullptr};

     public:
        shared_ptr() = default;

        ~shared_ptr() {
            if (control_block_ && control_block_->strong_ref_count.fetch_sub(1, std::memory_order::acq_rel) == 1) { delete control_block_; }
        }

        shared_ptr(T* value_ptr) {
            control_block_ = new __control_block<T>();
            control_block_->value = value_ptr;
        }

        shared_ptr(const T& value) {
            control_block_ = new __control_block<T>();
            control_block_->value = new T(value);
        }

        shared_ptr(T&& value) {
            control_block_ = new __control_block<T>();
            control_block_->value = new T(std::move(value));
        }

        shared_ptr(const shared_ptr& other) {
            control_block_ = other.control_block_;
            if (control_block_) { control_block_->strong_ref_count.fetch_add(1, std::memory_order::relaxed); }
        }

        shared_ptr(shared_ptr&& other) {
            control_block_ = other.control_block_;
            other.control_block_ = nullptr;
        }

        shared_ptr& operator=(const shared_ptr& other) {
            shared_ptr copy(other);
            std::swap(control_block_, copy.control_block_);
            return *this;
        }

        shared_ptr& operator=(shared_ptr&& other) {
            shared_ptr copy(std::move(other));
            std::swap(control_block_, copy.control_block_);
            return *this;
        }

        T* get() { return control_block_->value; }
        const T* get() const { return control_block_->value; }
        T* operator->() { return get(); }
        const T* operator->() const { return get(); }
        T& operator*() { return *get(); }
        const T& operator*() const { return *get(); }

        bool operator==(const shared_ptr& other) { return control_block_->value == other.control_block_->value; }
        bool operator==(const shared_ptr& other) const { return control_block_->value == other.control_block_->value; }
    };

}

int main() {
    senior::shared_ptr<int> shared(10);
    std::cout << *shared << '\n';
    *shared = 15;
    std::cout << *shared << '\n';
    return 0;
}