#pragma once

#include <WinSock2.h>

namespace cppio::win32
{
	using handle_t = void*;
	using socket_t = SOCKET;

	inline constexpr socket_t socket_error = SOCKET_ERROR;
	inline constexpr socket_t socket_invalid_handle = INVALID_SOCKET;
}