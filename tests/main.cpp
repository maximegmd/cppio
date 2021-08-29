#include "task.h"
#include "reactor.h"
#include "async_sleep.h"

namespace
{
    task<void> suspend_none()
    {
        std::printf("suspend_none\n");
        co_return;
    }

    task<int> suspend_one()
    {
        std::printf("suspend_one %d\\\n", std::this_thread::get_id());
        co_await std::suspend_always();
        std::printf("suspend_one /\n");
        co_return 1;
    }
    task<int> suspend_two()
    {
        co_await suspend_none();
        auto a = co_await suspend_one();
        co_await suspend_none();
        co_await async_sleep(std::chrono::seconds(1));
        auto b = co_await suspend_one();
        co_return a + b;
    }

    task<int> suspend_five()
    {
        auto a = co_await suspend_two();
        auto b = co_await suspend_two();
        co_return 1 + a + b;
    }

    task<int> run()
    {
        std::printf("run\n");
        auto a = co_await suspend_five();
        auto b = co_await suspend_five();
        auto c = co_await suspend_five();
        co_return 5 + a + b + c;
    }

}

int main()
{
    reactor pool(16);

    std::vector<std::unique_ptr<task<int>>> tasks;
    for (int i = 0; i < 1000; ++i)
    {
        tasks.emplace_back(std::make_unique<task<int>>(run()));
        pool.add(tasks[i].get());
    }

    pool.run();

    int sum = 0;

    for (auto& task : tasks)
    {
        sum += task->await_resume();
    }

    return sum;
}
