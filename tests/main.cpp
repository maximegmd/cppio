#include "task.h"
#include "reactor.h"
#include "async_sleep.h"
#include "async_tcp_socket.h"

#include <winsock2.h>

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

    task<void> tcp_echo(async_tcp_socket socket)
    {
        // Pretend to do something for now
        co_await async_sleep(std::chrono::seconds(5));
        std::printf("We sent some data yay!\n");
    }

    task<bool> tcp_test()
    {
        auto sock = async_tcp_socket::create_listener(12345);
        if (!sock.has_value())
            co_return false;

        std::vector<std::unique_ptr<task<void>>> tasks;
        while (true)
        {
            auto accepted = co_await sock->accept();
            std::printf("Accepted a socket!\n");
            tasks.emplace_back(std::make_unique<task<void>>(tcp_echo(std::move(*accepted))));

            reactor::get_current()->add((tasks.end() - 1)->get());
        }
        co_return true;
    }
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);

    reactor pool(16);

    auto task = tcp_test();

    pool.add(&task);
    pool.run();

    int sum = 0;

    return sum;
}
