#ifndef _ZHF_ASYNC_TIMER_WRAPPER_HPP_
#define _ZHF_ASYNC_TIMER_WRAPPER_HPP_
#pragma once

#include <unordered_set>

#include <asio.hpp>

namespace zhf {

class async_timer
{
	struct timer_wrapper {
		timer_wrapper(asio::io_context& io, std::size_t msecond) : timer_(io, std::chrono::milliseconds(msecond)) {}

		asio::steady_timer timer_;
		std::function<void(const std::error_code&)> task_wrapper_;
		std::atomic_bool is_running_{ true };
	};

public:
	~async_timer() { stop(); }

	template<std::size_t MSecond, typename F>
	auto post(F&& f, bool is_repeat = false) {
		static_assert(MSecond > 0, "The timing time of the async_timer must be greater than zero !!!");

		auto timer_ptr = std::make_shared<timer_wrapper>(io_context_, MSecond);
		timer_ptr->task_wrapper_ = [this, f = std::forward<F>(f), msecond = MSecond, is_repeat, timer_ptr](const std::error_code&) mutable {
			f();

			if (is_repeat && timer_ptr->is_running_.load(std::memory_order_acquire)) {
				timer_ptr->timer_.expires_after(std::chrono::milliseconds(msecond));
				timer_ptr->timer_.async_wait(timer_ptr->task_wrapper_);
			}
			else
			{
				std::lock_guard<std::mutex> lg(timers_mtx_);
				timers_.erase(timer_ptr);
			}
		};

		timer_ptr->timer_.async_wait(timer_ptr->task_wrapper_);
		{
			std::lock_guard<std::mutex> lg(timers_mtx_);
			timers_.insert(timer_ptr);
		}
		

		if (!is_running_.load(std::memory_order_acquire))
		{
			timer_thd_ = std::thread([this] { io_context_.run(); });
			is_running_.store(true, std::memory_order_release);
		}

		return timer_ptr;
	}

	template<typename T>
	void cancel(T&& timer_ptr) {
		std::error_code ec;
		timer_ptr->is_running_.store(false, std::memory_order_release);
		timer_ptr->timer_.cancel(ec);
		{
			std::lock_guard<std::mutex> lg(timers_mtx_);
			timers_.erase(timer_ptr);
		}
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
	asio::io_context::work work_{ io_context_ };
	std::mutex timers_mtx_;
	std::unordered_set<std::shared_ptr<timer_wrapper>> timers_;
	std::thread timer_thd_;
};

}
#endif