#pragma once

#include "reactor.h"

namespace cppio
{
	void initialize(size_t worker_count) noexcept;

	reactor* get() noexcept;

	template<class T>
	auto spawn(task<T> a_task)
	{
		return ::cppio::get()->spawn(std::move(a_task));
	}
}


   