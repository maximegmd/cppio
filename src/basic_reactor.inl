#ifndef BASIC_REACTOR_INL_DO
#error Do not include this file directly!
#endif

#include "task.hpp"
#include "basic_reactor.hpp"

namespace cppio
{
    template<class T>
    auto abstract_reactor::spawn(task<T> a_task)
    {
        auto p_task = std::make_shared<task<T>>(std::move(a_task));
        add(p_task);
        return p_task->get_future();
    }

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
            auto* p_old_basic_reactor = get_current();
            s_current_reactor = this;

            auto wait_time = m_timers.get_delay();
            if (m_tasks.empty() && wait_time == wait_time.zero())
            {
                wait_time = std::chrono::seconds(1);
            }

            m_completion_object.run(wait_time);

            if (!m_running)
                break;

            process_tasks();

            s_current_reactor = p_old_basic_reactor;
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
            std::shared_ptr<basic_task> p_task;
            {
                std::lock_guard _{ m_lock };
                if (m_tasks.empty())
                    break;
                p_task = m_tasks.front();
                m_tasks.pop_front();
            }

            if (!p_task)
                break;

            auto* p_old_task = s_current_task;
            s_current_task = p_task.get();

            auto result = basic_task::ScheduleType::kWait;
            if (!p_task->is_waiting())
                result = p_task->one_step();

            s_current_task = p_old_task;

            if (result == basic_task::ScheduleType::kRun)
            {
                std::lock_guard _{ m_lock };
                m_tasks.push_back(p_task);
            }
            if (result == basic_task::ScheduleType::kDone)
            {
                std::lock_guard _{ m_lock };
                m_active_tasks.erase(p_task);
            }
        }
    }
}