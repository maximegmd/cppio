#include "task.h"
#include "reactor.h"

void abstract_task::set_pool(reactor* pool) 
{ 
    m_pool = pool; 
}

void abstract_task::wake()
{
    m_waiting = false;
    m_pool->schedule(this);
}

void abstract_task::start_async_sleep(std::chrono::nanoseconds delay)
{
    m_waiting = true;
    m_pool->start_timer(delay, this);
}

bool abstract_task::is_waiting() const
{
    return m_waiting;
}

abstract_task::ScheduleType abstract_task::one_step()
{
    assert(!m_waiting);

    auto curr = get_promise_base();

    while (curr)
    {
        if (!curr.promise().m_inner_handler)
        {
            while (!curr.done() && !m_waiting)
            {
                curr.resume();
                if (!curr.done())
                {
                    return m_waiting ? ScheduleType::kWait : ScheduleType::kRun;
                }
                if (curr.promise().m_outer_handler)
                {
                    curr = curr.promise().m_outer_handler;
                    curr.promise().m_inner_handler = nullptr;
                }
                else
                {
                    return ScheduleType::kDone;
                }
            }
            break;
        }
        curr = curr.promise().m_inner_handler;
    }
    return curr.done() ? ScheduleType::kDone : (m_waiting ? ScheduleType::kWait : ScheduleType::kRun);
}
