#include <cppio/platform.hpp>
#include <cppio/reactor.hpp>
#include <cppio/network/udp_socket.hpp>
#include <cppio/impl/posix/basic_overlapped.hpp>

namespace cppio::network
{
    outcome::result<udp_socket> udp_socket::bind(const endpoint& local_endpoint) noexcept
	{
		auto sock = socket(local_endpoint.type(), SOCK_DGRAM, IPPROTO_UDP);
		if (sock == socket_error)
		{
			return network_error_code::SystemError;
		}

        int on = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == socket_error) 
        {
            close(sock);
            return network_error_code::SystemError;
        }

        // Don't share the socket
        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == socket_error)
        {
            close(sock);
            return network_error_code::SystemError;
        }

        // Set non blocking
        int flags = fcntl(sock, F_GETFL, 0);
        if (flags == socket_error)
        {
            close(sock);
            return network_error_code::SystemError;
        }

        flags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, flags) == socket_error)
        {
            close(sock);
            return network_error_code::SystemError;
        }

		sockaddr_storage addr;
		local_endpoint.fill(addr);

		if(::bind(sock, (sockaddr*)&addr, sizeof(addr)) != 0)
        {
			return network_error_code::Closed;
        }

		return udp_socket{ sock };
	}

	task<outcome::result<size_t>> udp_socket::read(endpoint& remote_endpoint, void* p_buffer, size_t size) noexcept
	{
        while(true)
        {
            sockaddr_storage from;
            socklen_t len = sizeof(from);

            auto result = recvfrom(m_socket, (char*)p_buffer, size, 0, (sockaddr*)&from, &len);

            if(result < 0 && errno != EINPROGRESS && errno != EWOULDBLOCK)
                co_return cppio::network_error_code::Closed;

            if (result > 0)
            {
                remote_endpoint = endpoint{ from };
                co_return result;
            }
            
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

	task<outcome::result<size_t>> udp_socket::write(const endpoint& remote_endpoint, const void* p_buffer, size_t size) noexcept
	{
		posix::basic_overlapped overlapped(posix::basic_overlapped::Type::kTcpSocket);

        auto write_sock = dup(m_out_socket);
        if(write_sock == socket_invalid_handle)
            co_return cppio::network_error_code::SystemError;

        sockaddr_storage to;
        remote_endpoint.fill(to);

        while(true)
        {        
            auto result = sendto(write_sock, (const char*)p_buffer, size, 0, (const sockaddr*)&to, sizeof(to));
            if(result < 0 && errno != EINPROGRESS && errno != EWOULDBLOCK)
                break;
            else if(result > 0)
            {
                close(write_sock);
                co_return result;
            }
            else
            {
                overlapped.task->wait();

                auto& completion_port = reactor::get_current()->get_completion_port();
                completion_port.add_for_write(write_sock, overlapped);

                co_await std::suspend_always{};

                if(!overlapped.success)
                    break;
                    
            }
        }

        close(write_sock);

        co_return cppio::network_error_code::Closed;
	}
}