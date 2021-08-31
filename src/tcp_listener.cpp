#include "tcp_listener.h"
#include "platform.h"

#include <algorithm>

#include "reactor.h"

namespace cppio
{
	tcp_listener::tcp_listener(socket_t socket)
		: tcp_socket(socket)
	{

	}
}