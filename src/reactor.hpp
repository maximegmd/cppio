#pragma once

#include "impl/basic_reactor.hpp"

#include "platform.hpp"

#if CPPIO_PLATFORM_WIN
#include "win32/iocp.hpp"
#elif CPPIO_PLATFORM_POSIX
#include "posix/epoll.hpp"
#endif

namespace cppio
{
#if CPPIO_PLATFORM_WIN
	struct reactor : impl::basic_reactor<win32::iocp>
#elif CPPIO_PLATFORM_POSIX
	struct reactor : impl::basic_reactor<posix::epoll>
#endif
	{
		reactor(size_t worker_count = 1) : basic_reactor(worker_count) {}
	};
}



   