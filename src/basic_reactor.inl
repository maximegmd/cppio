#ifndef BASIC_REACTOR_INL_DO
#error Do not include this file directly!
#endif

#include "task.h"
#include "basic_reactor.h"

template<class T>
basic_reactor<T>::basic_reactor(size_t worker_count)
    : m_running{ true }
{
    for (size_t i = 0; i < worker_count; ++i)
    {
        m_runners.emplace_back([this]()
        {
            run();
        });
    }
}

template<class T>
basic_reactor<T>::~basic_reactor()
{
    m_running = false;
}

template<class T>
void basic_reactor<T>::run()
{
    while (!m_active_tasks.empty())
    {
        auto wait_time = m_timers.get_delay();
        if (m_tasks.empty() && wait_time == wait_time.zero())
        {
            wait_time = std::chrono::seconds(1);
        }

        m_completion_object.run(wait_time);

        if (!m_running)
            break;

        process_tasks();
    }

    m_completion_object.notify_one();
}

template<class T>
basic_task* basic_reactor<T>::get_current_task()
{
    return s_current_task;
}

template<class T>
basic_reactor<T>* basic_reactor<T>::get_current()
{
    return s_current_reactor;
}

template<class T>
void basic_reactor<T>::process_tasks()
{
    while (!m_tasks.empty())
    {
        basic_task* p_task = nullptr;
        {
            std::lock_guard _{ m_lock };
            if (m_tasks.empty())
                break;
            p_task = m_tasks.front();
            m_tasks.pop_front();
        }

        auto* p_old_basic_reactor = get_current();
        auto* p_old_task = s_current_task;
        s_current_reactor = this;
        s_current_task = p_task;

        auto result = basic_task::ScheduleType::kWait;
        if (!p_task->is_waiting())
            result = p_task->one_step();

        s_current_task = p_old_task;
        s_current_reactor = p_old_basic_reactor;

        if (result == basic_task::ScheduleType::kRun)
        {
            std::lock_guard _{ m_lock };
            m_tasks.push_back(p_task);
        }
        if (result == basic_task::ScheduleType::kDone)
        {
            std::lock_guard _{ m_lock };
            m_active_tasks.erase(p_task->shared_from_this());
        }
    }
}
