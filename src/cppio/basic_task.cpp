#include <cppio/basic_task.hpp>
#include <cppio/reactor.hpp>

namespace cppio
{
    void basic_task::wake() noexcept
    {
        m_state = ScheduleType::kRun;
        reactor::get_current()->schedule(shared_from_this());
    }

    void basic_task::wait() noexcept
    {
        m_state = ScheduleType::kWait;
    }

    void basic_task::cancel_wait() noexcept
    {
        m_state = ScheduleType::kRun;
    }

    bool basic_task::is_waiting() const noexcept
    {
        return m_state == ScheduleType::kWait;
    }

    bool basic_task::await_suspend(std::coroutine_handle<> handle)
    {
        return true;
    }

    basic_task::ScheduleType basic_task::one_step()
    {
        assert(m_state.load() != ScheduleType::kWait);

        auto curr = get_promise_base();

        while (curr)
        {
            if (!curr.promise().m_inner_handle)
            {
                while (!curr.done() && !is_waiting())
                {
                    curr.resume();

                    if (!curr.done())
                    {
                        return m_state;
                    }
                    else if (curr.promise().m_outer_handle)
                    {
                        curr = curr.promise().m_outer_handle;
                        curr.promise().m_inner_handle = nullptr;
                    }
                    else
                    {
                        return ScheduleType::kDone;
                    }
                }
                break;
            }
            curr = curr.promise().m_inner_handle;
        }
        return curr.done() ? ScheduleType::kDone : m_state.load();
    }
}