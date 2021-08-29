#pragma once

#include <list>
#include <mutex>
#include <unordered_set>
#include <set>

struct basic_task;
struct timer_pool
{
	timer_pool();
	~timer_pool();

	void add(std::chrono::nanoseconds delay, basic_task* p_task);

	void process();
	std::chrono::nanoseconds get_delay();

private:

	struct delay_for
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> when;
		basic_task* p_task;

		bool operator<(const delay_for& rhs) const
		{
			return when < rhs.when;
		}
	};

	std::mutex m_lock;
	std::set<delay_for> m_tasks;
};

   