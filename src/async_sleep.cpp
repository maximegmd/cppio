#include "async_sleep.h"
#include "reactor.h"
#include "task.h"

bool async_sleep::await_suspend(std::coroutine_handle<> handle)
{
    auto p_task = reactor::get_current_task();

    p_task->start_async_sleep(m_delay);

    return true;
}
