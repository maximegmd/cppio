#pragma once

#include <cstdio>
#include <coroutine>
#include <optional>
#include <thread>
#include <cassert>
#include <concepts>
#include <future>

#include "basic_task.h"
#include "future.h"

namespace cppio
{
    struct abstract_reactor;

    template <class T, class U>
    concept Derived = std::is_base_of<U, T>::value;

    template <typename T = void>
    struct task final : basic_task
    {
        template<class T>
        struct final_task_promise;

        using pointer = std::shared_ptr<task>;
        using promise_type = final_task_promise<T>;
        using handle_type = std::coroutine_handle<promise_type>;

        task(handle_type handle)
            : m_handle(handle)
        {
        }

        task(task&& other) noexcept
            : m_handle(other.m_handle)
        {
            other.m_handle = nullptr;
        }

        virtual ~task()
        {
            if (m_handle)
                m_handle.destroy();
        }

        task(const task&) = delete;
        task& operator=(const task&) = delete;
        task& operator=(task&&) = delete;

        bool await_ready()
        {
            return !m_handle || m_handle.done();
        }

        auto get_future()
        {
            return cppio::future(std::static_pointer_cast<task<T>>(shared_from_this()));
        }

        template<Derived<promise_base> T>
        bool await_suspend(std::coroutine_handle<T> handle)
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
            return m_handle.promise().value();
        }

        void wait()
        {
            m_handle.promise().m_notifier.get_future().get();
        }

    private:

        mutable handle_type m_handle;

        struct task_promise : promise_base
        {
            std::promise<void> m_notifier{};

            auto initial_suspend()
            {
                return std::suspend_always{};
            }
        };

        template<class T>
        struct final_task_promise : task_promise
        {
            std::optional<T> m_value;

            auto value()
            {
                return std::move(*m_value);
            }

            void return_value(T&& t)
            {
                m_value = std::forward<T>(t);
                task_promise::m_notifier.set_value();
            }

            task<T> get_return_object()
            {
                return { handle_type::from_promise(*this) };
            }
        };

        template<>
        struct final_task_promise<void> : task_promise
        {
            void return_void()
            {
                task_promise::m_notifier.set_value();
            }

            task<T> get_return_object()
            {
                return { handle_type::from_promise(*this) };
            }
        };
    };
}