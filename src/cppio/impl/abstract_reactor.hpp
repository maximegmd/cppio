#pragma once

#include <list>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <thread>

#include <cppio/impl/timer_pool.hpp>

namespace cppio
{
    struct basic_task;

    template<class T>
    struct task;

    namespace impl
    {
        struct abstract_reactor
        {
            template<class T>
            auto spawn(task<T> p_task);

            void add(std::shared_ptr<basic_task> p_task);

            void schedule(std::shared_ptr<basic_task> p_task);
            void start_timer(std::chrono::nanoseconds delay, std::shared_ptr<basic_task> p_task);

        protected:

            std::mutex m_lock;
            std::list<std::shared_ptr<basic_task>> m_tasks;
            std::unordered_set<std::shared_ptr<basic_task>> m_active_tasks;
            timer_pool m_timers;
        };
    }
}

#define ABSTRACT_REACTOR_INL_DO
#include <cppio/impl/abstract_reactor.inl>
#undef ABSTRACT_REACTOR_INL_DO