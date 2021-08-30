#pragma once

#include <cstdio>
#include <coroutine>
#include <optional>
#include <thread>
#include <cassert>
#include <concepts>

namespace cppio
{
    struct abstract_reactor;

    struct basic_task : std::enable_shared_from_this<basic_task>
    {
        enum class ScheduleType
        {
            kDone,
            kRun,
            kWait
        };

        void wake();
        void wait();
        void start_async_sleep(std::chrono::nanoseconds delay);
        bool is_waiting() const;

        ScheduleType one_step();

        struct promise_base
        {
            std::coroutine_handle<promise_base> m_inner_handler{};
            std::coroutine_handle<promise_base> m_outer_handler{};

            auto final_suspend() noexcept
            {
                return std::suspend_always{};
            }

            void unhandled_exception()
            {
                std::terminate();
            }

            void rethrow_if_unhandled_exception()
            {
            }
        };

        virtual ~basic_task() {};
        virtual std::coroutine_handle<promise_base> get_promise_base() = 0;

    protected:

        std::atomic<bool> m_waiting{ false };
    };
}