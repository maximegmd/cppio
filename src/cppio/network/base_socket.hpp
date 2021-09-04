#pragma once

#include <cppio/platform.hpp>
#include <cppio/task.hpp>
#include <cppio/network/endpoint.hpp>

namespace cppio::network
{
    struct base_socket
    {
        base_socket() noexcept;
        base_socket(socket_t socket) noexcept;
        base_socket(const base_socket&) = delete;
        base_socket(base_socket&& rhs) noexcept;
        ~base_socket() noexcept;

        base_socket& operator=(const base_socket&) = delete;
        base_socket& operator=(base_socket&& rhs) noexcept;

        outcome::result<endpoint> local_endpoint() const noexcept;

    protected:

        outcome::result<endpoint> internal_remote_endpoint() const noexcept;

        friend struct tcp_listener;

        socket_t m_socket;
#if CPPIO_PLATFORM_POSIX
        socket_t m_out_socket;
#endif
    };
}