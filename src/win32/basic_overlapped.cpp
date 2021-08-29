#include "basic_overlapped.h"

namespace win32
{
	basic_overlapped::basic_overlapped(Type a_type)
		: type{ a_type }
	{
		std::memset(this, 0, sizeof(OVERLAPPED));
	}
}