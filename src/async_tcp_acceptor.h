#pragma once

#include "async_tcp_socket.h"

struct async_tcp_acceptor : async_tcp_socket
{
	static std::optional<async_tcp_acceptor> create(uint16_t port) noexcept;

	task<std::optional<async_tcp_socket>> accept();
};

   