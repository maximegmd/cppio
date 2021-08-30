#include "cppio.h"

namespace cppio
{
	static std::unique_ptr<reactor> s_reactor{};

	bool initialize(size_t worker_count) noexcept
	{
#if CPPIO_PLATFORM_WIN
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			return false;
#endif
		s_reactor = std::make_unique<reactor>(worker_count);

		return true;
	}

	reactor* get() noexcept
	{
		assert(s_reactor);

		return s_reactor.get();
	}

	void wait_for_all()
	{
		::cppio::get()->run();
	}
}