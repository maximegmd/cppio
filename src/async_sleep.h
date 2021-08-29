#pragma once

#include <coroutine>
#include <chrono>

struct async_sleep : std::suspend_always
{
    template<class T, class U>
    async_sleep(const std::chrono::duration<T, U>& delay) : m_delay(delay) {}

    bool await_suspend(std::coroutine_handle<> handle);

private:

    std::chrono::nanoseconds m_delay;
};