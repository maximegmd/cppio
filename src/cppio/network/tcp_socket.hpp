#pragma once

#include <cppio/network/base_socket.hpp>

namespace cppio::network
{
    struct tcp_socket : base_socket
    {
        static task<outcome::result<tcp_socket>> connect(const network::endpoint& endpoint) noexcept;

        tcp_socket() noexcept = default;
        tcp_socket(tcp_socket&& rhs) noexcept;
        tcp_socket& operator=(tcp_socket&& rhs) noexcept;
        ~tcp_socket() noexcept = default;

        tcp_socket(socket_t socket) noexcept;
        
        tcp_socket(const tcp_socket&) = delete;
        tcp_socket& operator=(const tcp_socket&) = delete;

        outcome::result<endpoint> remote_endpoint() const noexcept;

        task<outcome::result<size_t>> read(void* p_buffer, size_t size) noexcept;
        task<outcome::result<size_t>> read_exactly(void* p_buffer, size_t size) noexcept;
        task<outcome::result<size_t>> write(const void* p_buffer, size_t size) noexcept;
    };
}