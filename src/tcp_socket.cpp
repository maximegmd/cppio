#include "tcp_socket.h"
#include "reactor.h"
#include "win32/basic_overlapped.h"

#include <algorithm>

namespace cppio
{
	tcp_socket::tcp_socket() noexcept
		: m_socket{ INVALID_SOCKET }
	{
	}

	tcp_socket::tcp_socket(SOCKET sock) noexcept
		: m_socket{ sock }
	{
		if (sock != INVALID_SOCKET && sock != SOCKET_ERROR)
		{
			auto& r = reactor::get_current()->get_completion_port();
			r.add((handle_t)m_socket);
		}
	}

	tcp_socket::tcp_socket(tcp_socket&& rhs) noexcept
		: m_socket{ std::exchange(rhs.m_socket, INVALID_SOCKET) }
	{
	}

	tcp_socket::~tcp_socket() noexcept
	{
		if (m_socket != INVALID_SOCKET)
			closesocket(m_socket);
	}

	tcp_socket& tcp_socket::operator=(tcp_socket&& rhs) noexcept
	{
		std::swap(m_socket, rhs.m_socket);

		return *this;
	}

	task<size_t> tcp_socket::read(void* p_buffer, size_t size) noexcept
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
				co_return 0;
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

	task<size_t> tcp_socket::write(const void* p_buffer, size_t size) noexcept
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
				co_return 0;
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