#include <cppio/network/tcp_socket.hpp>

namespace cppio::network
{
	tcp_socket::tcp_socket(socket_t sock) noexcept
		: base_socket{ sock }
	{
	}

	tcp_socket::tcp_socket(tcp_socket&& rhs) noexcept
		: base_socket{ static_cast<base_socket&&>(rhs) }
	{
	}

	tcp_socket& tcp_socket::operator=(tcp_socket&& rhs) noexcept
	{
		base_socket::operator=(static_cast<base_socket&&>(rhs));
		return *this;
	}

	outcome::result<endpoint> tcp_socket::remote_endpoint() const noexcept
	{
		return internal_remote_endpoint();
	}
}