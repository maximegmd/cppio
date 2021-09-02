#include "abstract_reactor.hpp"
#include "task.hpp"

namespace cppio::impl
{
    void abstract_reactor::add(std::shared_ptr<basic_task> p_task)
    {
        std::lock_guard _{ m_lock };
        m_tasks.push_back(p_task);
        m_active_tasks.insert(p_task);
    }

    void abstract_reactor::schedule(std::shared_ptr<basic_task> p_task)
    {
        {
            std::lock_guard _{ m_lock };
            m_tasks.push_back(p_task);
        }
    }

    void abstract_reactor::start_timer(std::chrono::nanoseconds delay, std::shared_ptr<basic_task> p_task)
    {
        m_timers.add(delay, p_task);
    }
}