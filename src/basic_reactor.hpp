#pragma once

#include <list>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <thread>

#include "timer_pool.hpp"

namespace cppio
{
    struct basic_task;

    template<class T>
    struct task;

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

    template<class T>
    struct basic_reactor : abstract_reactor
    {
        basic_reactor(size_t worker_count = 1);
        ~basic_reactor();

        void run();

        static basic_task* get_current_task();
        static basic_reactor* get_current();
        T& get_completion_port() { return m_completion_object; }

    private:

        void process_tasks();

        T m_completion_object;
        std::atomic<bool> m_running;
        std::vector<std::jthread> m_runners;

        inline static thread_local basic_reactor* s_current_reactor;
        inline static thread_local basic_task* s_current_task;
    };
}

#define BASIC_REACTOR_INL_DO
#include "basic_reactor.inl"
#undef BASIC_REACTOR_INL_DO
