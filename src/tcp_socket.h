#pragma once

#include "platform.h"

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

	protected:

		friend struct tcp_listener;

		socket_t m_socket;
	};
}