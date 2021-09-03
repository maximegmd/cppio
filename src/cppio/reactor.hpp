#pragma once

#include <cppio/impl/basic_reactor.hpp>

#include <cppio/platform.hpp>

#if CPPIO_PLATFORM_WIN
#include <cppio/impl/win32/iocp.hpp>
#elif CPPIO_PLATFORM_POSIX
#include <cppio/impl/posix/epoll.hpp>
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



   