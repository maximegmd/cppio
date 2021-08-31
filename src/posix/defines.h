#pragma once

namespace cppio::posix
{
	using handle_t = int;
	using socket_t = int;

	inline constexpr socket_t socket_error = -1;
	inline constexpr socket_t socket_invalid_handle = -1;

	inline const auto closesocket = close;
}