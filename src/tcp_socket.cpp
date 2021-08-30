#include "tcp_socket.h"
#include "reactor.h"

#include <algorithm>

namespace cppio
{
	tcp_socket::tcp_socket() noexcept
		: m_socket{ INVALID_SOCKET }
	{
	}

	tcp_socket::tcp_socket(SOCKET sock) noexcept
		: m_socket{ sock }
	{
		if (sock != INVALID_SOCKET && sock != SOCKET_ERROR)
		{
			auto& r = reactor::get_current()->get_completion_port();
			r.add((handle_t)m_socket);
		}
	}

	tcp_socket::tcp_socket(tcp_socket&& rhs) noexcept
		: m_socket{ std::exchange(rhs.m_socket, INVALID_SOCKET) }
	{
	}

	tcp_socket::~tcp_socket() noexcept
	{
		if (m_socket != INVALID_SOCKET)
			closesocket(m_socket);
	}

	tcp_socket& tcp_socket::operator=(tcp_socket&& rhs) noexcept
	{
		std::swap(m_socket, rhs.m_socket);

		return *this;
	}
}