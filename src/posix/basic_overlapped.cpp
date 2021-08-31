#include "basic_overlapped.h"
#include "../reactor.h"

namespace cppio::posix
{
	basic_overlapped::basic_overlapped(Type a_type)
		: type{ a_type }
		, task{ reactor::get_current_task()->shared_from_this() }
	{
	}
}