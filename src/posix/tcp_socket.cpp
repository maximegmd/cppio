#include "../tcp_socket.hpp"
#include "../reactor.hpp"
#include "basic_overlapped.hpp"

#include <algorithm>

namespace cppio
{
	task<outcome::result<size_t>> tcp_socket::read(void* p_buffer, size_t size) noexcept
	{
		posix::basic_overlapped overlapped(posix::basic_overlapped::Type::kTcpSocket);

        overlapped.task->wait();

        auto& completion_port = reactor::get_current()->get_completion_port();
        completion_port.add_for_read(m_socket, overlapped);

        co_await std::suspend_always{};

        auto result = recv(m_socket, (char*)p_buffer, size, 0);
		if (result > 0)
		{
			co_return result;
		}
        else if(result < 0)
        {
            co_return cppio::socket_error_code::Closed;
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
            if(result > 0)
            {
                sent += result;
            }
            else if(result < 0)
            {
                co_return cppio::socket_error_code::Closed;
            }

            if(sent < size)
            {
                overlapped.task->wait();

                auto& completion_port = reactor::get_current()->get_completion_port();
                completion_port.add_for_write(m_out_socket, overlapped);

                co_await std::suspend_always{};
            }
        }

		co_return sent;
	}
}