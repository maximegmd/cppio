#include "sleep.h"
#include "reactor.h"
#include "task.h"

namespace cppio
{
    bool sleep::await_suspend(std::coroutine_handle<> handle)
    {
        auto p_task = reactor::get_current_task();

        p_task->start_async_sleep(m_delay);

        return true;
    }
}