// Implement a shared_ptr with an extension for weak_ptr
// `g++ -std=c++20 shared_ptr.cpp`

#include <concepts>
#include <utility>
#include <iostream>
#include <atomic>
#include <cassert>
#include <thread>
#include <vector>
#include <chrono>

namespace senior {

    namespace details {

        template <typename T>
        struct control_block {
            T* value{nullptr};
            std::atomic<int> strong_ref_count{1};
            std::atomic<int> weak_ref_count{1};
        };

    }

    template <typename T>
    class shared_ptr;

    template <typename T>
    class weak_ptr final {
        details::control_block<T>* control_block_{nullptr};

     public:
        weak_ptr()noexcept = default;
 
        ~weak_ptr() noexcept {
            if (control_block_ &&
                control_block_->weak_ref_count.fetch_sub(1, std::memory_order::acq_rel) == 1) {
                delete control_block_;
            }
        }

        weak_ptr(const shared_ptr<T>& shared) noexcept {
            control_block_ = shared.control_block_;
            if (control_block_) { control_block_->weak_ref_count.fetch_add(1, std::memory_order::relaxed); }
        }

        weak_ptr(const weak_ptr& other) noexcept {
            control_block_ = other.control_block_;
            if (control_block_) { control_block_->weak_ref_count.fetch_add(1, std::memory_order::relaxed); }
        }

        weak_ptr(weak_ptr&& other) noexcept {
            control_block_ = other.control_block_;
            other.control_block_ = nullptr;
        }

        weak_ptr& operator=(const weak_ptr& other) {
            weak_ptr copy(other);
            std::swap(control_block_, copy.control_block_);
            return *this;
        }

        weak_ptr& operator=(weak_ptr&& other) noexcept {
            weak_ptr copy(std::move(other));
            std::swap(control_block_, copy.control_block_);
            return *this;
        }

        shared_ptr<T> lock() noexcept {
            shared_ptr<T> shared;
            if (!control_block_) { return shared; }

            auto was_strong_ref_count = control_block_ ? control_block_->strong_ref_count.load(std::memory_order::acquire) : 0;
            bool locked = false;

            while (was_strong_ref_count > 0) {
                if (control_block_->strong_ref_count.compare_exchange_weak(was_strong_ref_count,
                                                                           was_strong_ref_count + 1,
                                                                           std::memory_order::release,
                                                                           std::memory_order::acquire)) {
                    locked = true;
                    break;
                }
            }

            if (locked) { shared.control_block_ = control_block_; }
            return shared;
        }
    };

    template <typename T>
    class shared_ptr final {
        friend class weak_ptr<T>;

        details::control_block<T>* control_block_{nullptr};

     public:
        shared_ptr() noexcept = default;

        ~shared_ptr() noexcept {
            if (control_block_ && control_block_->strong_ref_count.fetch_sub(1, std::memory_order::acq_rel) == 1) {
                delete control_block_->value;
                if (control_block_->weak_ref_count.fetch_sub(1, std::memory_order::acq_rel) == 1) { delete control_block_; }
            }
        }

        explicit shared_ptr(T* value_ptr) {
            control_block_ = new details::control_block<T>();
            control_block_->value = value_ptr;
        }

        explicit shared_ptr(const T& value) {
            control_block_ = new details::control_block<T>();
            control_block_->value = new T(value);
        }

        explicit shared_ptr(T&& value)  {
            control_block_ = new details::control_block<T>();
            control_block_->value = new T(std::move(value));
        }

        shared_ptr(const shared_ptr& other) noexcept {
            control_block_ = other.control_block_;
            if (control_block_) { control_block_->strong_ref_count.fetch_add(1, std::memory_order::relaxed); }
        }

        shared_ptr(shared_ptr&& other) noexcept {
            control_block_ = other.control_block_;
            other.control_block_ = nullptr;
        }

        shared_ptr& operator=(const shared_ptr& other) {
            shared_ptr copy(other);
            std::swap(control_block_, copy.control_block_);
            return *this;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept {
            shared_ptr copy(std::move(other));
            std::swap(control_block_, copy.control_block_);
            return *this;
        }

        T* get() noexcept { return control_block_ ? control_block_->value : nullptr; }
        const T* get() const noexcept { return control_block_ ? control_block_->value : nullptr; }
        T* operator->() noexcept { return get(); }
        const T* operator->() const noexcept { return get(); }
        T& operator*() { return *get(); }
        const T& operator*() const { return *get(); }

        bool operator==(const shared_ptr& other) const noexcept { return control_block_ == other.control_block_; }
        bool operator==(std::nullptr_t) const noexcept { return control_block_ == nullptr;  }
        explicit  operator bool() noexcept { return control_block_ != nullptr; }
    };

    struct Tracker {
        static inline std::atomic<int> count{0};
        int id;
        Tracker(int id) : id(id) { count++; }
        ~Tracker() { count--; }
    };

    void run_test() {
        std::cout << "--- Starting Complex Tests ---\n";

        // 1. Test on basic operations and semantics
        {
            shared_ptr<Tracker> shared_1(new Tracker(1));
            assert(Tracker::count == 1);
            {
                shared_ptr<Tracker> shared_2 = shared_1;
                assert(Tracker::count == 1);
                weak_ptr<Tracker> w1 = shared_2;
                auto shared_3 = w1.lock();
                assert(shared_3 == shared_1);
                assert(Tracker::count == 1);
            }
            assert(Tracker::count == 1);
        }
        assert(Tracker::count == 0); // Everything should be deleted
        std::cout << "Base test: PASSED\n";

        // 2. The “revival” (lock) test on a dead person
        {
            weak_ptr<Tracker> weak_orphan;
            {
                shared_ptr<Tracker> shared_temp(new Tracker(2));
                weak_orphan = shared_temp;
                assert(weak_orphan.lock() != nullptr);
            }
            assert(weak_orphan.lock() == nullptr); // The object has already been deleted
        }
        assert(Tracker::count == 0);
        std::cout << "Dead lock test: PASSED\n";

        // 3. STRESS TEST: Multithreading
        std::cout << "Starting multithreaded stress test (Race for lock/delete)...\n";
        {
            shared_ptr<Tracker> shared_main(new Tracker(42));
            weak_ptr<Tracker> weak_main = shared_main;

            std::vector<std::thread> threads;
            std::atomic<bool> start{false};
            std::atomic<int> success_locks{0};

            // Потоки пытаются лочить и копировать
            for (int i = 0; i < 10; ++i) {
                threads.emplace_back([&]() {
                    while (!start) { std::this_thread::yield(); }
                    for (int j = 0; j < 1000; ++j) {
                        auto locked = weak_main.lock();
                        if (locked) {
                            success_locks++;
                            shared_ptr<Tracker> shared_copy = locked;
                        }
                    }
                });
            }

            start = true;
            std::this_thread::sleep_for(std::chrono::microseconds(25)); 
            shared_main = shared_ptr<Tracker>(); // We kill the owner in the middle of his work

            for (auto& thread : threads) { thread.join(); }

            std::cout << "Successful locks during race: " << success_locks << "\n";
        }

        if (Tracker::count == 0) {
            std::cout << "Multithreaded test: PASSED (No leaks)\n";
        } else {
            std::cout << "Multithreaded test: FAILED (Leaked " << Tracker::count << " objects)\n";
        }
    }

}

// Explicit Instantiation (just to make sure the methods are syntactically correct)
template class senior::details::control_block<int>;
template class senior::weak_ptr<int>;
template class senior::shared_ptr<int>;

int main() {
    try {
        senior::run_test();
        std::cout << "All tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown crash!" << std::endl;
    }
    return 0;
}