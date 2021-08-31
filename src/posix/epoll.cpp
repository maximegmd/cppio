#include "../platform.h"
#include "epoll.h"
#include "../task.h"

namespace cppio::posix
{
	epoll::epoll()
	{
		m_handle = epoll_create1(0);
	}

	epoll::~epoll()
	{
		close(m_handle);
	}
	
	void epoll::run(const std::chrono::nanoseconds& wait_time) noexcept
	{
		epoll_event ev[10];
		
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(wait_time);
		auto result = epoll_wait(m_handle, ev, 10, ms.count());

		for(auto i = 0; i < result; ++i)
		{
			auto* p_overlapped = (basic_overlapped*)ev[i].data.ptr;
			p_overlapped->task->wake();
		}
	}

	bool epoll::add(handle_t handle) noexcept
	{
		return true;
	}

	bool epoll::add_for_read(handle_t handle, basic_overlapped& overlapped) noexcept
	{
		epoll_event ev;
		ev.events = EPOLLIN | EPOLLONESHOT;
		ev.data.ptr = &overlapped;

		if(epoll_ctl(m_handle, EPOLL_CTL_MOD, handle, &ev) != 0)
		{
			return epoll_ctl(m_handle, EPOLL_CTL_ADD, handle, &ev) == 0;
		}

		return true;
	}

	bool epoll::add_for_write(handle_t handle, basic_overlapped& overlapped) noexcept
	{
		epoll_event ev;
		ev.events = EPOLLOUT | EPOLLONESHOT;
		ev.data.ptr = &overlapped;

		if(epoll_ctl(m_handle, EPOLL_CTL_MOD, handle, &ev) != 0)
		{
			return epoll_ctl(m_handle, EPOLL_CTL_ADD, handle, &ev) == 0;
		}
	}

	void epoll::notify_one() noexcept
	{
		;
	}
}