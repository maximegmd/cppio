#pragma once

#include "task.h"

namespace cppio
{
    template<class T>
    struct task;

    template<class T>
    struct future
    {
        future(std::shared_ptr<task<T>> task) : m_task(std::move(task)) {}

        T get()
        {
            m_task->wait();

            return std::move(m_task->await_resume());
        }

    private:

        std::shared_ptr<task<T>> m_task;
    };
}