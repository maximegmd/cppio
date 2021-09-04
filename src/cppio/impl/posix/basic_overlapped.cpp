#include <cppio/impl/posix/basic_overlapped.hpp>
#include <cppio/reactor.hpp>

namespace cppio::posix
{
    basic_overlapped::basic_overlapped(Type a_type)
        : type{ a_type }
        , task{ reactor::get_current_task()->shared_from_this() }
    {
    }
}