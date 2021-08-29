#pragma once

#include <list>
#include <mutex>
#include <unordered_set>

#include "timer_pool.h"

struct abstract_task;

struct abstract_reactor
{
    void add(std::shared_ptr<abstract_task> p_task);
    void schedule(abstract_task* p_task);
    void start_timer(std::chrono::nanoseconds delay, abstract_task* p_task);

protected:

    std::mutex m_lock;
    std::list<abstract_task*> m_tasks;
    std::unordered_set<std::shared_ptr<abstract_task>> m_active_tasks;
    timer_pool m_timers;
};

template<class T>
struct basic_reactor : abstract_reactor
{
    basic_reactor(size_t worker_count = 1);
    ~basic_reactor();

    void run();
    void start_timer(std::chrono::nanoseconds delay, abstract_task* p_task);

    static abstract_task* get_current_task();
    static basic_reactor* get_current();
    T& get_completion_port() { return m_completion_object; }

private:

    void process_tasks();

    T m_completion_object;
    std::atomic<bool> m_running;
    std::vector<std::jthread> m_runners;

    inline static thread_local basic_reactor* s_current_reactor;
    inline static thread_local abstract_task* s_current_task;
};

#define BASIC_REACTOR_INL_DO
#include "basic_reactor.inl"
#undef BASIC_REACTOR_INL_DO