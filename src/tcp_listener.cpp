#include "tcp_listener.hpp"
#include "platform.hpp"

#include <algorithm>

#include "reactor.hpp"

namespace cppio
{
	tcp_listener::tcp_listener(socket_t socket)
		: tcp_socket(socket)
	{

	}
}