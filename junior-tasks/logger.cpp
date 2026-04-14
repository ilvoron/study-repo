// Write an implementation of the Singleton pattern
// `g++ -std=c++20 logger.cpp`

#include <string_view>
#include <iostream>
#include <mutex>
#include <chrono>
#include <functional>
#include <thread>
#include <condition_variable>
#include <deque>
#include <format>
#include <memory>
#include <atomic>

namespace junior {

class i_sink {
 public:
    virtual void write(const std::string& text) = 0;
    virtual ~i_sink() = default;
};

class standart_sink : public i_sink {
 public:
    ~standart_sink() { std::cout << std::flush; }
    void write(const std::string& text) override { std::cout << text << '\n'; }
};

class logger final {
    std::exception_ptr last_error_;
    std::atomic<int> active_tasks_{0};
    std::shared_ptr<i_sink> sink_;
    std::condition_variable_any new_log_;
    std::condition_variable wait_empty_;
    std::deque<std::string> queue_;
    std::mutex lock_;
    std::jthread worker_;

    logger() {
        worker_ = std::jthread([this](std::stop_token stop_token) { log_(stop_token); });
    }

    void log_(std::stop_token stop_token) {
        while (true) {
            std::string text{""};
            std::shared_ptr<i_sink> current_sink;

            {
                std::unique_lock guard(lock_);
                new_log_.wait(guard, stop_token, [this] { return !queue_.empty(); });
                if (queue_.empty() && stop_token.stop_requested()) { return; }

                if (!queue_.empty()) {
                    text = std::move(queue_.front());
                    queue_.pop_front();
                    current_sink = sink_;
                }
            }

            if (!text.empty()) {
                try {
                    if (current_sink) { current_sink->write(text); }
                } catch (...) {
                    std::lock_guard guard(lock_);
                    last_error_ = std::current_exception();
                }

                if (--active_tasks_ == 0) { wait_empty_.notify_all(); }
            }
        }
    }

    // Always stored last exception
    void check_and_throw_() {
        if (last_error_) {
            std::exception_ptr temp = nullptr;
            std::swap(last_error_, temp);
            std::rethrow_exception(temp);
        }
    }

 public:    
    static logger& instance() {
        static logger logger_;
        return logger_;
    }

    logger(const logger&) = delete;
    logger(logger&&) = delete;
    logger& operator=(const logger&) = delete;
    logger& operator=(logger&&) = delete;

    void set_sink(std::shared_ptr<i_sink> sink) {
        std::lock_guard guard(lock_);
        sink_ = sink;
    }

    void log(std::string_view text) {
        auto timestamp = std::chrono::system_clock::now();
        auto message = std::format("[{:%T}] {}", timestamp, text);

        {
            std::lock_guard guard(lock_);
            check_and_throw_();
            if (!sink_) { return; }

            ++active_tasks_;
            queue_.emplace_back(message);
        }

        new_log_.notify_one();
    }

    // Should be called before destroy, unless last exception couldn't be caught
    void wait_empty() {
        std::unique_lock guard(lock_);
        wait_empty_.wait(guard, [this] { return active_tasks_ == 0; });
        check_and_throw_();
    }
};

}

int main() {
    junior::logger& logger = junior::logger::instance();
    std::shared_ptr sink = std::make_shared<junior::standart_sink>();
    logger.set_sink(sink);
    logger.log("yay");
    logger.log("yay1");
    logger.wait_empty();
    return 0;
}