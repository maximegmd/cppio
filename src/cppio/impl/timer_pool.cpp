#include <cppio/impl/timer_pool.hpp>
#include <cppio/task.hpp>

#include <atomic>

namespace cppio::impl
{
	timer_pool::timer_pool()
	{
	}

	timer_pool::~timer_pool()
	{
	}

	void timer_pool::add(std::chrono::nanoseconds delay, std::shared_ptr<basic_task> p_task)
	{
		std::lock_guard _{ m_lock };
		m_tasks.insert({ std::chrono::high_resolution_clock::now() + delay, p_task });
	}

	void timer_pool::process()
	{
		// ignore result
		auto _ret = get_delay();
	}

	std::chrono::nanoseconds timer_pool::get_delay()
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds wait_time{ 0 };

		std::lock_guard _{ m_lock };

		while (!m_tasks.empty())
		{
			auto& task = *m_tasks.begin();
			if (task.when <= now)
			{
				task.p_task->wake();

				m_tasks.erase(m_tasks.begin());
			}
			else
			{
				wait_time = task.when - now;
				break;
			}
		}

		return wait_time;
	}
}