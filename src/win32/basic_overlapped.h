#pragma once

#include <chrono>
#include <mutex>
#include <winsock2.h>
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
		basic_task* task{ reactor::get_current_task() };
	};
}