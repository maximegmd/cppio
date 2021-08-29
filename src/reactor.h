#pragma once

#include <list>
#include <mutex>
#include <unordered_set>
#include <timer_pool.h>

struct abstract_task;
struct reactor
{
    reactor(size_t worker_count = 1);
    ~reactor();

    void add(abstract_task* p_task);
    void schedule(abstract_task* p_task);
    void run();
    void start_timer(std::chrono::nanoseconds delay, abstract_task* p_task);

    static abstract_task* get_current_task();
    static reactor* get_current();

private:

    void process_tasks();

    abstract_task* m_current_task;
    std::list<abstract_task*> m_tasks;
    std::unordered_set<abstract_task*> m_active_tasks;
    std::mutex m_lock;
    std::condition_variable m_cv;
    timer_pool m_timers;
    std::atomic<bool> m_running;
    std::vector<std::jthread> m_runners;

    static thread_local reactor* s_current_reactor;
    static thread_local abstract_task* s_current_task;
};

   