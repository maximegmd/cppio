#pragma once

#include <cppio/impl/abstract_reactor.hpp>

namespace cppio::impl
{  
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
#include <cppio/impl/basic_reactor.inl>
#undef BASIC_REACTOR_INL_DO
