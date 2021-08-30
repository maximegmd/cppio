#pragma once

#ifdef _WIN32
#define CPPIO_PLATFORM_WIN 1
#define CPPIO_PLATFORM_POSIX 0
#else
#define CPPIO_PLATFORM_WIN 0
#define CPPIO_PLATFORM_POSIX 1
#endif

#if CPPIO_PLATFORM_WIN
#include <WinSock2.h>

#include "win32/defines.h"
namespace cppio
{
	using win32::handle_t;
	using win32::socket_t;
}
#endif

#if CPPIO_PLATFORM_POSIX
#include "posix/defines.h"

namespace cppio
{
	using handle_t = posix::handle_t;
}
#endif
   