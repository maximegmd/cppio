#pragma once

#include <chrono>
#include <mutex>

#include "defines.hpp"

namespace cppio::win32
{
	struct iocp
	{
		iocp();
		~iocp();

		void run(const std::chrono::nanoseconds& wait_time) noexcept;
		bool add(handle_t handle) noexcept;
		void notify_one() noexcept;

	private:

		handle_t m_handle;
	};
}