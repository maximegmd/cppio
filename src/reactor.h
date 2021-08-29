#pragma once

#include "basic_reactor.h"

#ifdef _WIN32
#include "win32/iocp.h"

struct reactor : basic_reactor<win32::iocp>
#endif
{
	reactor(size_t worker_count = 1) : basic_reactor(worker_count) {}
};



   