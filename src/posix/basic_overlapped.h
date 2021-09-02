#pragma once

#include "../platform.h"
#include <memory>

#if CPPIO_PLATFORM_POSIX

namespace cppio
{
    struct basic_task;
}

namespace cppio::posix
{
	struct basic_overlapped
	{
		enum class Type
		{
			kTcpSocket
		};

		basic_overlapped(Type a_type);

		Type type;
		std::shared_ptr<basic_task> task;
	};
}

#endif