#include "tcp_socket.h"
#include "reactor.h"
#include "win32/basic_overlapped.h"

#include <algorithm>

namespace cppio
{
	tcp_socket::tcp_socket() noexcept
		: m_socket{ socket_invalid_handle }
#if CPPIO_PLATFORM_POSIX
		, m_out_socket { socket_invalid_handle }
#endif
	{
	}

	tcp_socket::tcp_socket(socket_t sock) noexcept
		: m_socket{ sock }
	{
		if (sock != socket_invalid_handle && sock != socket_error)
		{
#if CPPIO_PLATFORM_POSIX
			m_out_socket = dup(m_socket);
#endif

			auto& r = reactor::get_current()->get_completion_port();
			r.add((handle_t)m_socket);
		}
	}

	tcp_socket::tcp_socket(tcp_socket&& rhs) noexcept
		: m_socket{ std::exchange(rhs.m_socket, socket_invalid_handle) }
#if CPPIO_PLATFORM_POSIX
		, m_out_socket{ std::exchange(rhs.m_out_socket, socket_invalid_handle) }
#endif
	{
	}

	tcp_socket::~tcp_socket() noexcept
	{
#if CPPIO_PLATFORM_POSIX
		if (m_out_socket != socket_invalid_handle)
			closesocket(m_out_socket);
#endif

		if (m_socket != socket_invalid_handle)
			closesocket(m_socket);
	}

	tcp_socket& tcp_socket::operator=(tcp_socket&& rhs) noexcept
	{
		std::swap(m_socket, rhs.m_socket);

		return *this;
	}
}