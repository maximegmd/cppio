#include <cppio/network/tcp_listener.hpp>
#include <cppio/impl/win32/basic_overlapped.hpp>

namespace cppio::network
{
    outcome::result<tcp_listener> tcp_listener::create(const endpoint& local_endpoint) noexcept
	{
		auto sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sock == SOCKET_ERROR)
		{
			return network_error_code::SystemError;
		}

		sockaddr_storage addr;
		local_endpoint.fill(addr);		

		if (bind(sock, (const sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			return network_error_code::SystemError;
		}

		listen(sock, 100);

		return std::move(tcp_listener(sock));
	}

	task<outcome::result<tcp_socket>> tcp_listener::accept()
	{
		win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

		tcp_socket out(WSASocketA(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED));

		// Oops we didn't get a socket...
		if (out.m_socket == SOCKET_ERROR)
		{
			co_return network_error_code::SystemError;
		}

		char addr_buff[1024];
		DWORD bytes_read = 0;

		// We start waiting
		overlapped.task->wait();

		while (true)
		{
			auto result = AcceptEx(m_socket, out.m_socket, &addr_buff, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes_read, &overlapped);

			auto err = WSAGetLastError();
			// If we started the IO, suspend and wait to be resumed
			if (result == TRUE || err == ERROR_IO_PENDING)
			{
				co_await std::suspend_always{};

				if (overlapped.success)
				{
					setsockopt(out.m_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_socket, sizeof(m_socket));

					co_return outcome::success(std::move(out));
				}
			}
			// If the error is not a client side reset, we abort
			else if (err != WSAECONNRESET)
			{
				break;
			}
		}

		co_return network_error_code::SystemError;
	}
}