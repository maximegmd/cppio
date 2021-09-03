#include <cppio/platform.hpp>
#include <cppio/network/tcp_socket.hpp>
#include <cppio/reactor.hpp>
#include <cppio/impl/posix/basic_overlapped.hpp>


#include <algorithm>

namespace cppio::network
{
    task<outcome::result<tcp_socket>> tcp_socket::connect(const network::endpoint& endpoint) noexcept
	{
		auto sock = socket(endpoint.type(), SOCK_STREAM, 0);
		if (sock == socket_error)
			co_return network_error_code::SystemError;

		// Don't share the socket
        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == socket_error)
        {
            close(sock);
            co_return network_error_code::SystemError;
        }

        // Set non blocking
        int flags = fcntl(sock, F_GETFL, 0);
        if (flags == socket_error)
        {
            close(sock);
            co_return network_error_code::SystemError;
        }

        flags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, flags) == socket_error)
        {
            close(sock);
            co_return network_error_code::SystemError;
        }

		sockaddr_storage addr;
		endpoint.fill(addr);

        auto result = ::connect(sock, (sockaddr*)&addr, sizeof(addr));

        if(result < 0 && errno != EINPROGRESS && errno != EWOULDBLOCK)
            co_return network_error_code::Closed;

        if(result == 0)
            co_return tcp_socket{ sock };

        posix::basic_overlapped overlapped(posix::basic_overlapped::Type::kTcpSocket);
        auto& completion_port = reactor::get_current()->get_completion_port();
        completion_port.add_for_write(sock, overlapped);

        overlapped.task->wait();

        co_await std::suspend_always{};

        if(overlapped.success)
            co_return tcp_socket{ sock };

		co_return network_error_code::SystemError;
	}

	task<outcome::result<size_t>> tcp_socket::read(void* p_buffer, size_t size) noexcept
	{
        while(true)
        {
            auto result = recv(m_socket, (char*)p_buffer, size, 0);

            if(result < 0 && errno != EINPROGRESS && errno != EWOULDBLOCK)
                co_return cppio::network_error_code::Closed;

            if (result > 0)
                co_return result;
            
            posix::basic_overlapped overlapped(posix::basic_overlapped::Type::kTcpSocket);

            overlapped.task->wait();

            auto& completion_port = reactor::get_current()->get_completion_port();
            completion_port.add_for_read(m_socket, overlapped);

            co_await std::suspend_always{};

            if(!overlapped.success)
                co_return cppio::network_error_code::Closed;
        }

		co_return 0;
	}

	task<outcome::result<size_t>> tcp_socket::write(const void* p_buffer, size_t size) noexcept
	{
		posix::basic_overlapped overlapped(posix::basic_overlapped::Type::kTcpSocket);
        
        size_t sent = 0;
        while(sent < size)
        {
            auto result = send(m_out_socket, ((const char*)p_buffer + sent), size - sent, 0);
            if(result < 0 && errno != EINPROGRESS && errno != EWOULDBLOCK)
                co_return cppio::network_error_code::Closed;
            else if(result > 0)
                sent += result;

            if(sent < size)
            {
                overlapped.task->wait();

                auto& completion_port = reactor::get_current()->get_completion_port();
                completion_port.add_for_write(m_out_socket, overlapped);

                co_await std::suspend_always{};

                if(!overlapped.success)
                    co_return cppio::network_error_code::Closed;
            }
        }

		co_return sent;
	}
}