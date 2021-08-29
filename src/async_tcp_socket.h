#pragma once

#include <optional>
#include <WinSock2.h>
#include "task.h"
#include "win32/basic_overlapped.h"

struct async_tcp_socket
{
	async_tcp_socket() noexcept;
	async_tcp_socket(SOCKET sock) noexcept;
	async_tcp_socket(const async_tcp_socket&) = delete;
	async_tcp_socket(async_tcp_socket&& rhs) noexcept;
	~async_tcp_socket() noexcept;

	async_tcp_socket& operator=(const async_tcp_socket&) = delete;
	async_tcp_socket& operator=(async_tcp_socket&& rhs) noexcept;

	static std::optional<async_tcp_socket> create_listener(uint16_t port) noexcept;

	task<std::optional<async_tcp_socket>> accept();

private:

	friend struct async_tcp_socket_acceptor;

	SOCKET m_sock;
	void* m_handle{ INVALID_HANDLE_VALUE };
};

   