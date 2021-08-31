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
#include <mswsock.h>

#include "win32/defines.h"
namespace cppio
{
	using win32::handle_t;
	using win32::socket_t;

	using win32::socket_invalid_handle;
	using win32::socket_error;
}
#endif

#if CPPIO_PLATFORM_POSIX
#include <unistd.h> 
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include "posix/defines.h"

namespace cppio
{
	using posix::handle_t;
	using posix::socket_t;
	using posix::closesocket;

	using posix::socket_invalid_handle;
	using posix::socket_error;
}
#endif
   