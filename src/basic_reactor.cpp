#include "basic_reactor.h"
#include "task.h"

void abstract_reactor::add(std::shared_ptr<abstract_task> p_task)
{
    {
        std::lock_guard _{ m_lock };
        m_tasks.push_back(p_task.get());
        m_active_tasks.insert(p_task);
    }

    p_task->set_pool(this);
}

void abstract_reactor::schedule(abstract_task* p_task)
{
    {
        std::lock_guard _{ m_lock };
        m_tasks.push_back(p_task);
    }
}

void abstract_reactor::start_timer(std::chrono::nanoseconds delay, abstract_task* p_task)
{
    m_timers.add(delay, p_task);
}