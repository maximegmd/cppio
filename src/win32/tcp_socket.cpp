#include "../tcp_socket.hpp"
#include "../reactor.hpp"
#include "basic_overlapped.hpp"

#include <algorithm>

namespace cppio
{
	task<outcome::result<size_t>> tcp_socket::read(void* p_buffer, size_t size) noexcept
	{
		WSABUF wsa_buf;
		wsa_buf.len = size;
		wsa_buf.buf = (CHAR*)p_buffer;

		DWORD flags = 0, bytes_read;

		win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

		overlapped.task->wait();

		if (WSARecv(m_socket, &wsa_buf, 1, &bytes_read, &flags, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING) 
			{		
				co_return cppio::socket_error_code::Closed;
			}
		}

		// WSARecv can return immediately, but the overlapped event will still be scheduled, if we don't suspend
		// this coroutine will be destroyed when the overlapped event is processed and the overlapped structure
		// will be invalid memory, so even if we get an immediate result we wait for the overlapped event to
		// run.
		co_await std::suspend_always{};

		if (overlapped.success)
			co_return std::move(overlapped.bytes_transferred);

		co_return 0;
	}

	task<outcome::result<size_t>> tcp_socket::write(const void* p_buffer, size_t size) noexcept
	{
		WSABUF wsa_buf;
		wsa_buf.len = size;
		wsa_buf.buf = (CHAR*)p_buffer;

		DWORD bytes_sent;

		win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

		overlapped.task->wait();

		if (WSASend(m_socket, &wsa_buf, 1, &bytes_sent, 0, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				co_return cppio::socket_error_code::Closed;
			}
		}

		// WSASend can return immediately, but the overlapped event will still be scheduled, if we don't suspend
		// this coroutine will be destroyed when the overlapped event is processed and the overlapped structure
		// will be invalid memory, so even if we get an immediate result we wait for the overlapped event to
		// run.
		co_await std::suspend_always{};

		if (overlapped.success)
			co_return std::move(overlapped.bytes_transferred);

		co_return 0;
	}
}