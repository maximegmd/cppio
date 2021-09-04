#include <cppio/network/udp_socket.hpp>

namespace cppio::network
{
    udp_socket::udp_socket(socket_t sock) noexcept
        : base_socket{ sock }
    {
    }

    udp_socket::udp_socket(udp_socket&& rhs) noexcept
        : base_socket{ static_cast<base_socket&&>(rhs) }
    {
    }

    udp_socket& udp_socket::operator=(udp_socket&& rhs) noexcept
    {
        base_socket::operator=(static_cast<base_socket&&>(rhs));
        return *this;
    }
}