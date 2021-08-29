#pragma once

#include <chrono>
#include <mutex>

namespace win32
{
	struct iocp
	{
		iocp();
		~iocp();

		void run(const std::chrono::nanoseconds& wait_time);
		void notify_one();
		void* get_handle() const;

	private:

		std::mutex m_lock;
		void* m_handle;
	};
}