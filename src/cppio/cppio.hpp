#pragma once

#include <cppio/reactor.hpp>

namespace cppio
{
	[[nodiscard]] bool initialize(size_t worker_count) noexcept;

	reactor* get() noexcept;

	template<class T>
	auto spawn(task<T> a_task)
	{
		return ::cppio::get()->spawn(std::move(a_task));
	}

	void wait_for_all();
}


   