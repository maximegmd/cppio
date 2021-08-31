#pragma once

#include "../platform.h"

#if CPPIO_PLATFORM_WIN

#include "../reactor.h"

namespace cppio::win32
{
	struct basic_overlapped : OVERLAPPED
	{
		enum class Type
		{
			kTcpSocket
		};

		basic_overlapped(Type a_type);

		Type type;
		bool success{ false };
		size_t bytes_transferred{ 0 };
		std::shared_ptr<basic_task> task{ reactor::get_current_task()->shared_from_this() };
	};
}

#endif