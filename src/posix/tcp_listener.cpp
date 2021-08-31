#include "../platform.h"
#include "../tcp_listener.h"
#include "../reactor.h"

#include "basic_overlapped.h"

namespace cppio
{
    std::optional<tcp_listener> tcp_listener::create(uint16_t port) noexcept
	{
		auto sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == socket_error)
		{
			return std::nullopt;
		}

        int on = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == socket_error) 
        {
            close(sock);
            return std::nullopt;
        }

        // 
        if (setsockopt(sock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &on, sizeof(on)) == socket_error)
        {
            close(sock);
            return std::nullopt;
        }

        // Don't share the socket
        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == socket_error)
        {
            close(sock);
            return std::nullopt;
        }

        // Set non blocking
        int flags = fcntl(sock, F_GETFL, 0);
        if (flags == socket_error)
        {
            close(sock);
            return std::nullopt;
        }

        flags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, flags) == socket_error)
        {
            close(sock);
            return std::nullopt;
        }

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if (bind(sock, (const sockaddr*)&addr, sizeof(addr)) == socket_error)
		{
			return std::nullopt;
		}

		listen(sock, 100);

		return std::move(tcp_listener(sock));
	}

	task<std::optional<tcp_socket>> tcp_listener::accept()
	{
        posix::basic_overlapped ov(posix::basic_overlapped::Type::kTcpSocket);

        auto& completion_port = reactor::get_current()->get_completion_port();
        completion_port.add_for_read(m_socket, ov);

        ov.task->wait();

        co_await std::suspend_always{};

        sockaddr addr;
        socklen_t len = sizeof(addr);

        auto sock = accept4(m_socket, (sockaddr*)&addr, &len, SOCK_NONBLOCK|SOCK_CLOEXEC);
        if(sock > -1)
        {
            co_return tcp_socket{ sock };
        }

		co_return std::nullopt;
	}
}