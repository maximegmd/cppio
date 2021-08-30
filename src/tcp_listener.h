#pragma once

#include "tcp_socket.h"
#include "task.h"

#include <optional>

namespace cppio
{
	struct tcp_listener : tcp_socket
	{
		tcp_listener(const tcp_listener&) = delete;
		tcp_listener(tcp_listener&&) = default;
		tcp_listener& operator=(const tcp_listener&) = delete;
		tcp_listener& operator=(tcp_listener&&) = default;
		tcp_listener(socket_t socket);

		static std::optional<tcp_listener> create(uint16_t port) noexcept;

		task<std::optional<tcp_socket>> accept();
	};
}