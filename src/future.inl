#ifndef BASIC_FUTURE_INL_DO
#error Do not include this file directly!
#endif

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