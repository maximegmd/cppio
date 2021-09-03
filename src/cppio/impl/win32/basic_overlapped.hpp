#pragma once

#include <cppio/platform.hpp>

#if CPPIO_PLATFORM_WIN

#include <cppio/reactor.hpp>

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