#include <cppio/network/tcp_socket.hpp>
#include <cppio/impl/win32/basic_overlapped.hpp>

namespace cppio::network
{
	task<outcome::result<tcp_socket>> tcp_socket::connect(const network::endpoint& endpoint) noexcept
	{
		auto out = tcp_socket{ WSASocketA(endpoint.type(), SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED) };
		if (out.m_socket == socket_invalid_handle)
			co_return network_error_code::SystemError;

		sockaddr_storage addr;
		std::memset(&addr, 0, sizeof(addr));
		addr.ss_family = endpoint.type();
		if (addr.ss_family == AF_INET)
		{
			((sockaddr_in*)&addr)->sin_port = 0;
			((sockaddr_in*)&addr)->sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			((sockaddr_in6*)&addr)->sin6_port = 0;
			((sockaddr_in6*)&addr)->sin6_addr = in6addr_any;
		}

		auto rc = bind(out.m_socket, (sockaddr*)&addr, sizeof(addr));
		if (rc != 0) 
			co_return network_error_code::SystemError;

		DWORD num_bytes = 0;
		GUID guid = WSAID_CONNECTEX;
		LPFN_CONNECTEX ConnectExPtr = nullptr;

		int success = WSAIoctl(out.m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			(void*)&guid, sizeof(guid), (void*)&ConnectExPtr, sizeof(ConnectExPtr),
			&num_bytes, nullptr, nullptr);

		if(!ConnectExPtr)
			co_return network_error_code::SystemError;

		win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

		endpoint.fill(addr);

		overlapped.task->wait();

		auto result = ConnectExPtr(out.m_socket, (sockaddr*)&addr, sizeof(addr), nullptr, 0, nullptr, &overlapped);
		auto error = WSAGetLastError();

		if(result == TRUE || error == ERROR_IO_PENDING)
		{
			co_await std::suspend_always{};

			setsockopt(out.m_socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, nullptr, 0);

			if (overlapped.success)
				co_return std::move(out);
		}

		co_return network_error_code::SystemError;
	}

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
				co_return cppio::network_error_code::Closed;
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
		win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

		size_t total = 0;

		while (total < size)
		{
			DWORD bytes_sent = 0;

			WSABUF wsa_buf;
			wsa_buf.len = size - total;
			wsa_buf.buf = ((CHAR*)p_buffer) + total;

			if (WSASend(m_socket, &wsa_buf, 1, &bytes_sent, 0, &overlapped, nullptr) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					co_return cppio::network_error_code::Closed;
				}
			}

			overlapped.task->wait();
			// WSASend can return immediately, but the overlapped event will still be scheduled, if we don't suspend
			// this coroutine will be destroyed when the overlapped event is processed and the overlapped structure
			// will be invalid memory, so even if we get an immediate result we wait for the overlapped event to
			// run.
			co_await std::suspend_always{};

			if (overlapped.success)
				total += overlapped.bytes_transferred;
			else
				co_return cppio::network_error_code::Closed;
		}

		co_return total;
	}
}