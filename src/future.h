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

#include "task.h"

namespace cppio
{
    template<class T>
    T future<T>::get() noexcept
    {
        if (!m_task)
            return {};

        m_task->wait();

        auto res = std::move(m_task->await_resume());

        m_task = nullptr;

        return res;
    }
}