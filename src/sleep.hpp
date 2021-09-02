#pragma once

#include <coroutine>
#include <chrono>

namespace cppio
{
    struct sleep : std::suspend_always
    {
        template<class T, class U>
        sleep(const std::chrono::duration<T, U>& delay) : m_delay(delay) {}

        bool await_suspend(std::coroutine_handle<> handle);

    private:

        std::chrono::nanoseconds m_delay;
    };
}