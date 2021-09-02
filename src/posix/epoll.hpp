#pragma once

#include <chrono>

#include "defines.hpp"
#include "basic_overlapped.hpp"

namespace cppio::posix
{
	struct epoll
	{
		epoll();
		~epoll();

		epoll& operator=(const epoll&) = delete;
		epoll& operator=(epoll&&) = delete;
		epoll(const epoll&) = delete;
		epoll(epoll&&) = delete;

		void run(const std::chrono::nanoseconds& wait_time) noexcept;
		bool add(handle_t handle) noexcept;

		bool add_for_read(handle_t handle, basic_overlapped& overlapped) noexcept;
		bool add_for_write(handle_t handle, basic_overlapped& overlapped) noexcept;
		void notify_one() noexcept;

	private:

		handle_t m_handle;
	};
}