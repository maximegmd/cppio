#include "async_tcp_acceptor.h"

#include <algorithm>
#include <mswsock.h>

#include "win32/basic_overlapped.h"
#include "reactor.h"

std::optional<async_tcp_acceptor> async_tcp_acceptor::create(uint16_t port) noexcept
{
	auto sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock == SOCKET_ERROR)
	{
		return std::nullopt;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (bind(sock, (const sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		return std::nullopt;
	}

	listen(sock, 100);

	return std::move(async_tcp_acceptor(sock));
}

task<std::optional<async_tcp_socket>> async_tcp_acceptor::accept()
{
	win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

	async_tcp_socket out(WSASocketA(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED));

	// Oops we didn't get a socket...
	if (out.m_sock == SOCKET_ERROR)
	{
		co_return std::nullopt;
	}

	char addr_buff[1024];
	DWORD bytes_read = 0;

	// We start waiting
	overlapped.task->wait();

	while (true)
	{
		auto result = AcceptEx(m_sock, out.m_sock, &addr_buff, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes_read, &overlapped);
		// Lucky us we had a connection ready, don't need to wait (we don't set the task's wait state because it's dead on return anyway)
		if (result == TRUE)
		{
			co_return std::move(out);
		}

		auto err = WSAGetLastError();
		// If we started the IO, suspend and wait to be resumed
		if (err == ERROR_IO_PENDING)
		{
			co_await std::suspend_always{};
			if (overlapped.success)
				co_return std::move(out);
		}
		// If the error is not a client side reset, we abort
		else if (err != WSAECONNRESET)
		{
			break;
		}
	}

	co_return std::nullopt;
}
