#pragma once

#include <cstdio>
#include <coroutine>
#include <optional>
#include <thread>
#include <cassert>
#include <concepts>

#include "basic_task.h"

struct abstract_reactor;

template <class T, class U>
concept Derived = std::is_base_of<U, T>::value;

template <typename T = void>
struct task final : basic_task
{
    struct task_promise;

    using pointer = std::shared_ptr<task>;
    using promise_type = task_promise;
    using handle_type = std::coroutine_handle<promise_type>;

    mutable handle_type m_handle;

    task(handle_type handle)
        : m_handle(handle)
    {
    }

    task(task&& other) noexcept
        : m_handle(other.m_handle)
    {
        other.m_handle = nullptr;
    }

    task& operator=(const task&) = delete;
    task& operator=(task&&) = delete;

    pointer to_owned()
    {
        return std::make_shared<task>(std::move(*this));
    }

    bool await_ready()
    {
        return !m_handle || m_handle.done();
    }

    template<Derived<promise_base> T>
    bool await_suspend(std::coroutine_handle<T> handle)
    {
        handle.promise().m_inner_handler = std::coroutine_handle<promise_base>::from_address(m_handle.address());
        m_handle.promise().m_outer_handler = std::coroutine_handle<promise_base>::from_address(handle.address());
        return true;
    }

    bool await_suspend(std::coroutine_handle<> handle)
    {
        return true;
    }

    bool await_suspend(std::coroutine_handle<promise_type> handle)
    {
        handle.promise().m_inner_handler = std::coroutine_handle<promise_base>::from_address(m_handle.address());
        m_handle.promise().m_outer_handler = std::coroutine_handle<promise_base>::from_address(handle.address());
        return true;
    }

    std::coroutine_handle<promise_base> get_promise_base() override
    {
        return std::coroutine_handle<promise_base>::from_address(m_handle.address());
    }

    auto await_resume()
    {
        return std::move(*m_handle.promise().m_value);
    }

    virtual ~task()
    {
        if (m_handle)
            m_handle.destroy();
    }

    struct task_promise : promise_base
    {
        std::optional<T> m_value{};

        auto value()
        {
            return m_value;
        }

        auto initial_suspend()
        {
            return std::suspend_always{};
        }

        void return_value(T&& t)
        {
            m_value = std::forward<T>(t);
        }

        task<T> get_return_object()
        {
            return { handle_type::from_promise(*this) };
        }
    };
};

template <>
struct task<void> final : basic_task
{
    struct task_promise;

    using pointer = std::shared_ptr<task>;
    using promise_type = task_promise;
    using handle_type = std::coroutine_handle<promise_type>;

    mutable handle_type m_handle;

    task(handle_type handle)
        : m_handle(handle)
    {}

    task(task&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr))
    {}

    pointer to_owned()
    {
        return std::make_shared<task>(std::move(*this));
    }

    bool await_ready()
    {
        return !m_handle || m_handle.done();
    }

    template<Derived<promise_base> T>
    bool await_suspend(std::coroutine_handle<T> handle)
    {
        handle.promise().m_inner_handler = std::coroutine_handle<promise_base>::from_address(m_handle.address());
        m_handle.promise().m_outer_handler = std::coroutine_handle<promise_base>::from_address(handle.address());
        return true;
    }

    bool await_suspend(std::coroutine_handle<> handle)
    {
        return true;
    }

    bool await_suspend(std::coroutine_handle<promise_type> handle)
    {
        handle.promise().m_inner_handler = std::coroutine_handle<promise_base>::from_address(m_handle.address());
        m_handle.promise().m_outer_handler = std::coroutine_handle<promise_base>::from_address(handle.address());
        return true;
    }

    std::coroutine_handle<promise_base> get_promise_base() override
    {
        return std::coroutine_handle<promise_base>::from_address(m_handle.address());
    }

    auto await_resume()
    {
        return 0;
    }

    virtual ~task()
    {
        if (m_handle)
            m_handle.destroy();
    }

    struct task_promise : promise_base
    {
        auto initial_suspend()
        {
            return std::suspend_always{};
        }

        void return_void()
        {
        }

        task<void> get_return_object()
        {
            return { handle_type::from_promise(*this) };
        }
    };
};