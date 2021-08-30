#include "basic_task.h"
#include "reactor.h"

namespace cppio
{
    void basic_task::wake()
    {
        m_waiting = false;
        reactor::get_current()->schedule(this);
    }

    void basic_task::wait()
    {
        m_waiting = true;
    }

    void basic_task::start_async_sleep(std::chrono::nanoseconds delay)
    {
        wait();
        reactor::get_current()->start_timer(delay, this);
    }

    bool basic_task::is_waiting() const
    {
        return m_waiting;
    }

    basic_task::ScheduleType basic_task::one_step()
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
}