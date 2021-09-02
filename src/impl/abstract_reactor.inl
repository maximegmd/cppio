#ifndef ABSTRACT_REACTOR_INL_DO
#error Do not include this file directly!
#endif

#include "task.hpp"
#include "abstract_reactor.hpp"

namespace cppio::impl
{
    template<class T>
    auto abstract_reactor::spawn(task<T> a_task)
    {
        auto p_task = std::make_shared<task<T>>(std::move(a_task));
        add(p_task);
        return p_task->get_future();
    }
}