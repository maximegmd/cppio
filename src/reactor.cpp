#include "reactor.h"
#include "task.h"

thread_local reactor* reactor::s_current_reactor = nullptr;
thread_local abstract_task* reactor::s_current_task = nullptr;

reactor::reactor(size_t worker_count)
    : m_current_task{nullptr}
    , m_running{true}
{
    worker_count = std::max(worker_count, size_t{ 1 });

    for (size_t i = 0; i < worker_count; ++i)
    {
        m_runners.emplace_back([this]()
        {
            run();
        });
    }
}

reactor::~reactor()
{
    m_running = false;

    m_cv.notify_all();
}

void reactor::add(abstract_task* p_task)
{
    {
        std::lock_guard _{ m_lock };
        m_tasks.push_back(p_task);
        m_active_tasks.insert(p_task);
    }

    p_task->set_pool(this);

    m_cv.notify_one();
}

void reactor::schedule(abstract_task* p_task)
{
    {
        std::lock_guard _{ m_lock };
        m_tasks.push_back(p_task);
    }

    m_cv.notify_one();
}

void reactor::run()
{
    while (!m_active_tasks.empty())
    {
        auto wait_time = m_timers.get_delay();

        {
            std::unique_lock lk{ m_lock };

            if (m_tasks.empty())
            {
                m_cv.wait_for(lk, wait_time);
            }

            if (!m_running)
                return;
        }

        process_tasks();        
    }
}

void reactor::start_timer(std::chrono::nanoseconds delay, abstract_task* p_task)
{
    m_timers.add(delay, p_task);
}

abstract_task* reactor::get_current_task()
{
    return s_current_task;
}

reactor* reactor::get_current()
{
    return s_current_reactor;
}

void reactor::process_tasks()
{
    while (!m_tasks.empty())
    {
        abstract_task* p_task = nullptr;
        {
            std::lock_guard _{ m_lock };
            if (m_tasks.empty())
                break;
            p_task = m_tasks.front();
            m_tasks.pop_front();
        }

        auto* p_old_reactor = s_current_reactor;
        auto* p_old_task = s_current_task;
        s_current_reactor = this;
        s_current_task = p_task;

        auto result = abstract_task::ScheduleType::kWait;
        if (!p_task->is_waiting())
            result = p_task->one_step();

        s_current_task = p_old_task;
        s_current_reactor = p_old_reactor;

        if (result == abstract_task::ScheduleType::kRun)
        {
            std::lock_guard _{ m_lock };
            m_tasks.push_back(p_task);
        }
        if (result == abstract_task::ScheduleType::kDone)
        {
            {
                std::lock_guard _{ m_lock };
                m_active_tasks.erase(p_task);
            }
            m_cv.notify_all();
        }
    }
}
