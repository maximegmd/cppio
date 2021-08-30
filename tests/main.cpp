#include "task.h"
#include "reactor.h"
#include "sleep.h"
#include "tcp_socket.h"
#include "tcp_listener.h"
#include "cppio.h"

using cppio::task;

namespace
{
    task<std::string> build_response(const std::string& data)
    {
        // Pretend we are doing some database work or something
        co_await cppio::sleep(std::chrono::seconds(1));

        auto response = R"V0G0N(HTTP/1.1 200 OK
Date: Mon, 27 Jul 2009 12:28:53 GMT
Server: cppio/0.0.1
Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT
Content-Length: ")V0G0N" + std::to_string(data.size()) + 
R"V0G0N(Content-Type: text/html
Connection: Closed

)V0G0N" + data;

        co_return std::move(response);
    }

    task<> http_hello(cppio::tcp_socket socket)
    {
        char data[1025];
        auto read = co_await socket.read(data, 1024);
        data[read] = 0;

        static std::atomic<int> counter = 0;

        std::string content = "<html><h1>Hi " + std::to_string(++counter) + "!</h1></html>";
        auto response = co_await build_response(content);

        auto sent = co_await socket.write(response.c_str(), response.size());
    }

    task<bool> http_test()
    {
        auto sock = cppio::tcp_listener::create(12345);
        if (!sock)
            co_return false;

        while (true)
        {
            auto accepted = co_await sock->accept();

            if (accepted)
            {
                std::printf("New TCP connection!\n");

                cppio::spawn(http_hello(std::move(*accepted)));
            }
        }

        co_return true;
    }
}

int main()
{
    if (!cppio::initialize(16))
        return -1;

    auto result = cppio::spawn(http_test()).get();

    return 0;
}
