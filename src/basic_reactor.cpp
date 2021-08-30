#include "basic_reactor.h"
#include "task.h"

namespace cppio
{
    void abstract_reactor::add(std::shared_ptr<basic_task> p_task)
    {
        std::lock_guard _{ m_lock };
        m_tasks.push_back(p_task.get());
        m_active_tasks.insert(p_task);
    }

    void abstract_reactor::schedule(basic_task* p_task)
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