#pragma once

#include <unordered_set>
#include <boost/asio.hpp>


namespace nbc {
    using namespace boost;
    class async_timer final
    {
    public:
        class timer_wrapper final : public std::enable_shared_from_this<timer_wrapper> {
        public:
            friend class async_timer;
            timer_wrapper(asio::io_context& io, std::size_t msecond, bool repeat = false) : timer_(io, std::chrono::milliseconds(msecond))
                , msecond_(msecond), repeat_(repeat) {
            }
            ~timer_wrapper() = default;

            void work_fn() {
                auto self(shared_from_this());
                timer_.expires_after(std::chrono::milliseconds(msecond_));
                timer_.async_wait([self](const std::error_code& ec) {
                    if (ec) {
                        return;
                    }
                    self->task_wrapper_();
                    if (self->repeat_ && self->is_running_) {
                        self->work_fn();
                    }
                    });
            }
        private:
            asio::steady_timer timer_;
            std::function<void()> task_wrapper_;
            std::atomic_bool is_running_{ true };
            bool repeat_;
            std::size_t msecond_;
        };

    public:
        async_timer() = default;
        ~async_timer() { stop(); }

        template<std::size_t MSecond, typename F>
        auto post(F&& f, bool is_repeat = false) -> std::shared_ptr< timer_wrapper> {
            static_assert(MSecond > 0, "The timing time of the async_timer must be greater than zero !!!");

            auto timer_ptr = std::make_shared<timer_wrapper>(io_context_, MSecond, is_repeat);
            timer_ptr->task_wrapper_ = std::forward<F>(f);

            timer_ptr->work_fn();

            if (!is_running_.load(std::memory_order_acquire))
            {
                timer_thd_ = std::thread([this] { io_context_.run(); });
                is_running_.store(true, std::memory_order_release);
            }

            return timer_ptr;
        }

        template<typename T>
        void cancel(T&& timer_ptr) {
            timer_ptr->is_running_.store(false, std::memory_order_release);
            timer_ptr->timer_.cancel();
        }

        void stop() {
            if (!is_running_.load(std::memory_order_acquire))
                return;
            is_running_.store(false, std::memory_order_release);

            io_context_.stop();
            timer_thd_.join();
        }

    private:
        std::atomic_bool is_running_{ false };
        asio::io_context io_context_;
        asio::io_context::work  work_{ io_context_ };
        std::mutex timers_mtx_;
        std::thread timer_thd_;
    };

}
