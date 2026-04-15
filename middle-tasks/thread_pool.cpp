// Write a thread-safe thread pool
// `g++ -std=c++20 thread_pool.cpp`

#include <thread>
#include <vector>
#include <atomic>
#include <future>
#include <deque>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <random>
#include <cstdint>
#include <mutex>
#include <exception>

namespace middle {

    class thread_pool final {
        std::deque<std::function<void()>> tasks_;
        std::atomic<bool> stop_{false}; // we can use jthread, but because we check flag in `add_task` it's better to keep this way, unless it's token and atomic to store same condition
        std::condition_variable cond_;
        std::mutex lock_;
        std::vector<std::thread> pool_;

        void worker_thread() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock guard(lock_);
                    cond_.wait(guard, [this] { return !tasks_.empty() || stop_; });
                    if (stop_ && tasks_.empty()) { return; }
                    task = std::move(tasks_.front());
                    tasks_.pop_front();
                }
                task();
            }
        }

     public:
        thread_pool(std::size_t threads_count) {
            pool_.reserve(threads_count);
            for (std::size_t i = 0; i < threads_count; ++i) {
                pool_.emplace_back([this] { worker_thread(); });
            }
        }

        ~thread_pool() {
            stop_ = true;
            cond_.notify_all();
            for (auto& thread : pool_) {
                if (thread.joinable()) { thread.join(); }
            }
        }

        thread_pool(const thread_pool&) = delete;
        thread_pool(thread_pool&&) = delete;
        void operator=(const thread_pool&) = delete;
        void operator=(thread_pool&&) = delete;

        template <typename F, typename... Args>
        auto add_task(F&& func, Args&&... args) ->
         std::future<std::invoke_result_t<F, Args...>> {
            auto task = std::make_shared<std::packaged_task<std::invoke_result_t<F, Args...>()>>(
             [func = std::forward<F>(func),
              ...args = std::forward<Args>(args)]() mutable {
                return std::invoke(std::move(func), std::move(args)...);
            });
            auto result = task->get_future();
            {
                std::lock_guard guard(lock_);
                if (stop_) { throw std::runtime_error("Adding task to a stopped thread pool"); }
                tasks_.emplace_back([task] { (*task)(); });
            }
            cond_.notify_one();
            return result;
        }
    };

    std::int64_t heavy_task(std::int64_t count) {
        volatile std::int64_t counter = 0;
        while (counter < count) { counter = counter + 1; }
        return counter;
    }

    std::int64_t heavy_task_beauty(std::int64_t count, int id) {
        auto result = middle::heavy_task(count);
        std::cout << "Task " << id << " finished! Result: " << result << '\n';
        return result;
    }

}

int main() {
    std::vector<std::future<std::int64_t>> results;
    results.reserve(5);

    {
        middle::thread_pool pool(10);
        std::cout << "thread_pool(10) created\n";
        results.push_back(pool.add_task(middle::heavy_task_beauty, 100000000, 1));
        results.push_back(pool.add_task(middle::heavy_task_beauty, 2000000, 2));
        results.push_back(pool.add_task(middle::heavy_task_beauty, 30000000, 3));
        results.push_back(pool.add_task(middle::heavy_task_beauty, 4000000000, 4));
        results.push_back(pool.add_task(middle::heavy_task_beauty, 5000000, 5));
        std::cout << "Tasks added!\n";
    }

    std::cout << "Results: ";
    for (auto& result : results) {
        std::cout << result.get() << ' ';
    }
    std::cout << "\n\n";

    results.clear();
    results.reserve(std::thread::hardware_concurrency());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::int64_t> dist(10000000, 5000000000);
    {
        middle::thread_pool pool(std::thread::hardware_concurrency());
        std::cout << "thread_pool(" << std::thread::hardware_concurrency() << ") created\n";
        for (std::size_t i = 1; i <= std::thread::hardware_concurrency(); ++i) {
            results.push_back(pool.add_task(middle::heavy_task_beauty, dist(gen), i));
        }
        std::cout << "Tasks added!\n";
    }

    std::cout << "Results: ";
    for (auto& result : results) {
        std::cout << result.get() << ' ';
    }

    return 0;
}