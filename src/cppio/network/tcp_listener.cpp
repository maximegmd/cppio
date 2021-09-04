#include <cppio/network/tcp_listener.hpp>

namespace cppio::network
{
    tcp_listener::tcp_listener(socket_t socket)
        : tcp_socket(socket)
    {
    }
}