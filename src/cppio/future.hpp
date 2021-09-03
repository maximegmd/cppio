#pragma once

#include <memory>

namespace cppio
{
    template<class T>
    struct task;

    template<class T>
    struct future
    {
        future(std::shared_ptr<task<T>> task) : m_task(std::move(task)) {}

        T get() noexcept;

    private:

        std::shared_ptr<task<T>> m_task;
    };
}

#define BASIC_FUTURE_INL_DO
#include <cppio/future.inl>
#undef BASIC_FUTURE_INL_DO