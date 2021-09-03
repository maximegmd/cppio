#include <cppio/network/tcp_socket.hpp>
#include <cppio/reactor.hpp>
#include <cppio/impl/win32/basic_overlapped.hpp>

#include <algorithm>

namespace cppio::network
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
		if (sock != socket_invalid_handle)
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
	
	outcome::result<endpoint> tcp_socket::local_endpoint() const noexcept
	{
		sockaddr_storage result;
		socklen_t len = sizeof(result);
		if (getsockname(m_socket, (sockaddr*)&result, &len) != 0)
			return network_error_code::Closed;

		if (result.ss_family == AF_INET6)
		{
			auto* addr = (sockaddr_in6*)&result;
			return network::endpoint{ ip::v6{ addr->sin6_addr.s6_addr }, ntohs(addr->sin6_port) };
		}

		auto* addr = (sockaddr_in*)&result;
		return network::endpoint{ ip::v4{ ntohl(addr->sin_addr.s_addr) }, ntohs(addr->sin_port) };
	}

	outcome::result<endpoint> tcp_socket::remote_endpoint() const noexcept
	{
		sockaddr_storage result;
		socklen_t len = sizeof(result);
		if (getpeername(m_socket, (sockaddr*)&result, &len) != 0)
			return network_error_code::Closed;

		if (result.ss_family == AF_INET6)
		{
			auto* addr = (sockaddr_in6*)&result;
			return network::endpoint{ ip::v6{ addr->sin6_addr.s6_addr }, ntohs(addr->sin6_port) };
		}

		auto* addr = (sockaddr_in*)&result;
		return network::endpoint{ ip::v4{ ntohl(addr->sin_addr.s_addr) }, ntohs(addr->sin_port) };
	}
}