#include "async_tcp_socket.h"
#include "reactor.h"

#include <algorithm>

async_tcp_socket::async_tcp_socket() noexcept
	: m_sock{INVALID_SOCKET}
{
}

async_tcp_socket::async_tcp_socket(SOCKET sock) noexcept
	: m_sock{sock}
{
	if (sock != INVALID_SOCKET && sock != SOCKET_ERROR)
	{
		auto& r = reactor::get_current()->get_completion_port();
		auto handle = CreateIoCompletionPort((HANDLE)m_sock, r.get_handle(), 0, 0);
	}
}

async_tcp_socket::async_tcp_socket(async_tcp_socket&& rhs) noexcept
	: m_sock{ std::exchange(rhs.m_sock, INVALID_SOCKET) }
{
}

async_tcp_socket::~async_tcp_socket() noexcept
{
	if (m_sock != INVALID_SOCKET)
		closesocket(m_sock);
}

async_tcp_socket& async_tcp_socket::operator=(async_tcp_socket&& rhs) noexcept
{
	std::swap(m_sock, rhs.m_sock);

	return *this;
}
