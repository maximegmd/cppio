#include <cppio/basic_task.hpp>
#include <cppio/reactor.hpp>

namespace cppio
{
    void basic_task::wake() noexcept
    {
        m_waiting = false;
        reactor::get_current()->schedule(shared_from_this());
    }

    void basic_task::wait() noexcept
    {
        m_waiting = true;
    }

    void basic_task::cancel_wait() noexcept
    {
        m_waiting = false;
    }

    bool basic_task::is_waiting() const noexcept
    {
        return m_waiting;
    }

    bool basic_task::await_suspend(std::coroutine_handle<> handle)
    {
        return true;
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