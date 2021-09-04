#include <cppio/network/udp_socket.hpp>
#include <cppio/impl/win32/basic_overlapped.hpp>
#include <cppio/reactor.hpp>

namespace cppio::network
{
    outcome::result<udp_socket> udp_socket::bind(const endpoint& local_endpoint) noexcept
    {
        auto socket = WSASocket(local_endpoint.type(), SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (socket == socket_invalid_handle)
            return network_error_code::SystemError;

        sockaddr_storage addr;
        local_endpoint.fill(addr);

        if (::bind(socket, (SOCKADDR*)&addr, sizeof(addr)) != 0)
            return network_error_code::Closed;

        BOOL new_behavior = FALSE;
        DWORD dw_bytes_returned = 0;
        WSAIoctl(socket, SIO_UDP_CONNRESET, &new_behavior, sizeof(new_behavior), NULL, 0, &dw_bytes_returned, NULL, NULL);

        return udp_socket{ socket };
    }

    task<outcome::result<size_t>> udp_socket::read(endpoint& remote_endpoint, void* p_buffer, size_t size) noexcept
    {
        WSABUF wsa_buf;
        wsa_buf.len = size;
        wsa_buf.buf = (CHAR*)p_buffer;

        DWORD flags = 0, bytes_read;

        win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

        overlapped.task->wait();

        sockaddr_storage from;
        int len = sizeof(from);

        if (WSARecvFrom(m_socket, &wsa_buf, 1, &bytes_read, &flags, (sockaddr*)&from, &len, &overlapped, nullptr) == SOCKET_ERROR)
        {
            auto err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {        
                co_return cppio::network_error_code::Closed;
            }
        }

        // WSARecvFrom can return immediately, but the overlapped event will still be scheduled, if we don't suspend
        // this coroutine will be destroyed when the overlapped event is processed and the overlapped structure
        // will be invalid memory, so even if we get an immediate result we wait for the overlapped event to
        // run.
        co_await std::suspend_always{};

        if (overlapped.success)
        {
            remote_endpoint = endpoint{ from };
            co_return std::move(overlapped.bytes_transferred);
        }

        co_return 0;
    }

    task<outcome::result<size_t>> udp_socket::write(const endpoint& remote_endpoint, const void* p_buffer, size_t size) noexcept
    {
        WSABUF wsa_buf;
        wsa_buf.len = size;
        wsa_buf.buf = (CHAR*)p_buffer;

        DWORD bytes_sent = 0;

        win32::basic_overlapped overlapped(win32::basic_overlapped::Type::kTcpSocket);

        sockaddr_storage addr;
        remote_endpoint.fill(addr);

        overlapped.task->wait();

        if (WSASendTo(m_socket, &wsa_buf, 1, &bytes_sent, 0, (sockaddr*)&addr, sizeof(addr), &overlapped, nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                co_return cppio::network_error_code::Closed;
            }
        }

        // WSASendTo can return immediately, but the overlapped event will still be scheduled, if we don't suspend
        // this coroutine will be destroyed when the overlapped event is processed and the overlapped structure
        // will be invalid memory, so even if we get an immediate result we wait for the overlapped event to
        // run.
        co_await std::suspend_always{};

        if (overlapped.success)
            co_return std::move(overlapped.bytes_transferred);

        co_return cppio::network_error_code::Closed;
    }
}