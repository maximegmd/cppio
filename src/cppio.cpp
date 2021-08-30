#include "cppio.h"

#ifdef _WIN32
#include <WinSock2.h>
#endif

namespace cppio
{
	static std::unique_ptr<reactor> s_reactor{};

	void initialize(size_t worker_count) noexcept
	{
#if _WIN32
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
		s_reactor = std::make_unique<reactor>(worker_count);
	}

	reactor* get() noexcept
	{
		return s_reactor.get();
	}
}