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

	task<outcome::result<size_t>> tcp_socket::read_exactly(void* p_buffer, size_t size) noexcept
	{
		size_t total_read = 0;

		while (total_read != size)
		{
			auto read_res = co_await read((char*)p_buffer + total_read, size - total_read);
			if (!read_res)
				co_return cppio::network_error_code::Closed;
			total_read += read_res.value();
		}

		co_return 0;
	}
}