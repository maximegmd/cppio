#include "task.h"
#include "reactor.h"
#include "sleep.h"
#include "tcp_socket.h"
#include "tcp_listener.h"
#include "cppio.h"

using cppio::task;

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
        co_await cppio::sleep(std::chrono::seconds(1));
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

    task<> tcp_echo(cppio::tcp_socket socket)
    {
        // Pretend to do something for now
        co_await cppio::sleep(std::chrono::seconds(5));
        std::printf("We sent some data yay!\n");
    }

    task<bool> tcp_test()
    {
        auto sock = cppio::tcp_listener::create(12345);
        if (!sock)
            co_return false;

        while (true)
        {
            auto accepted = co_await sock->accept();

            if (accepted)
            {
                std::printf("Accepted a socket!\n");

                cppio::spawn(tcp_echo(std::move(*accepted)));
            }
        }

        co_return true;
    }
}

int main()
{
    cppio::initialize(16);

    auto result = cppio::spawn(tcp_test());
    auto wait = result.get();

    return 0;
}
