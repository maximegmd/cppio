#include "sleep.hpp"
#include "reactor.hpp"
#include "task.hpp"

namespace cppio
{
    bool sleep::await_suspend(std::coroutine_handle<> handle)
    {
        auto p_reactor = reactor::get_current();
        auto p_task = reactor::get_current_task();

        p_task->wait();
        p_reactor->start_timer(m_delay, p_task->shared_from_this());

        return true;
    }
}