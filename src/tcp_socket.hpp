#pragma once

#include "platform.hpp"
#include "task.hpp"

namespace cppio
{
	struct tcp_socket
	{
		tcp_socket() noexcept;
		tcp_socket(socket_t socket) noexcept;
		tcp_socket(const tcp_socket&) = delete;
		tcp_socket(tcp_socket&& rhs) noexcept;
		~tcp_socket() noexcept;

		tcp_socket& operator=(const tcp_socket&) = delete;
		tcp_socket& operator=(tcp_socket&& rhs) noexcept;

		task<outcome::result<size_t>> read(void* p_buffer, size_t size) noexcept;
		task<outcome::result<size_t>> write(const void* p_buffer, size_t size) noexcept;

	protected:

		friend struct tcp_listener;

		socket_t m_socket;
#if CPPIO_PLATFORM_POSIX
		socket_t m_out_socket;
#endif
	};
}