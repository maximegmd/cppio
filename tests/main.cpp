#include <cppio/task.hpp>
#include <cppio/sleep.hpp>
#include <cppio/network/tcp_socket.hpp>
#include <cppio/network/udp_socket.hpp>
#include <cppio/network/tcp_listener.hpp>
#include <cppio/network/http/request.hpp>
#include <cppio/network/http/response.hpp>
#include <cppio/cppio.hpp>
#include <iostream>

using cppio::task;

namespace
{
    task<std::string> build_response(const std::string& data)
    {
        // Pretend we are doing some database work or something
        co_await cppio::sleep(std::chrono::seconds(1));

        cppio::network::http::response r;
        r.headers().add("Date", "Mon, 27 Jul 2009 12:28:53 GMT");
        r.headers().add("Server", "cppio/0.0.1");
        r.headers().add("Last-Modified", "Wed, 22 Jul 2009 19:15:56 GMT");
        r.headers().add("Content-Type", "text/html");
        r.headers().add("Connection", "Closed");
        r.set_content("<html><h1>Hi !</h1></html>");

        co_return r.serialize();
    }

    task<> http_hello(cppio::network::tcp_socket connection)
    {
        char data[4097];
        // Very simple read, we try to get at most 4096 bytes
        auto read_res = co_await connection.read(data, 4096);
        if(!read_res)
            co_return;

        auto read = read_res.value();
        data[read] = 0;

        auto request = cppio::network::http::request::create(data);
        if (!request)
        {
            std::printf("Malformed request!\n");
            co_return;
        }

        static std::atomic<int> counter = 0;

        std::string content = "<html><h1>Hi " + std::to_string(++counter) + "!</h1></html>";

        // We can await the response, this could be disk i/o, database access etc.
        auto response = co_await build_response(content);

        // And now that we resumed we can send the result.
        auto sent = co_await connection.write(response.c_str(), response.size());
    }

    task<bool> http_test()
    {
        // Try to listen on port 12345.
        auto listener_result = cppio::network::tcp_listener::create(cppio::network::endpoint::parse("0.0.0.0:12345").value());
        if (!listener_result)
            co_return false;

        auto listener = std::move(listener_result.value());

        // Serve 5 requests then stop.
        for(int i = 0; i < 5; ++i)
        {
            // Wait for a connection, this is blocking in the coroutine but the coroutine 
            // itself will be paused until a connection is accepted, yielding execution
            // to other coroutines, if any.
            auto new_connection = co_await listener.accept();

            if (new_connection)
            {
                // Unwrap directly there is no reason for this to fail here
                auto endpoint = new_connection.value().remote_endpoint().value();

                std::printf("New TCP connection from %s\n", endpoint.to_string().c_str());

                // We spawn another task, we don't want to block incoming connections, if we were good citizens
                // we could obtain the future returned here and wait for it to return before returning ourself
                // but cppio::wait_for_all() in main() will take care of waiting for everything to complete.
                cppio::spawn(http_hello(std::move(new_connection.value())));
            }
        }

        co_return true;
    }

    task<bool> client_test()
    {
        co_await cppio::sleep{ std::chrono::milliseconds(500) };
        auto endpoint = cppio::network::endpoint::parse("127.0.0.1:12345");
        if (!endpoint)
            co_return false;

        auto sock_result = co_await cppio::network::tcp_socket::connect(endpoint.value());
        if (!sock_result)
            co_return false;

        auto sock = std::move(sock_result.value());
        auto sent_result = co_await sock.write("test", 4);

        char buff[1025];
        auto recv_result = co_await sock.read(buff, 1024);

        if (!recv_result)
            co_return false;

        buff[recv_result.value()] = 0;
        std::printf("%s\n", buff);

        co_return true;
    }

    task<bool> udp_server()
    {
        auto endpoint = cppio::network::endpoint::parse("[::]:12346");
        if (!endpoint)
            co_return false;

        auto server_res = cppio::network::udp_socket::bind(endpoint.value());
        if (!server_res)
            co_return false;

        auto server = std::move(server_res.value());

        while (true)
        {
            cppio::network::endpoint from;
            char data[1025];
            auto read_res = co_await server.read(from, data, 1024);
            if (read_res)
            {
                data[read_res.value()] = 0;
                std::printf("UDP read from %s: %s\n", from.to_string().c_str(), data);

                const char* rdata = "bye!";
                auto write_res = co_await server.write(from, rdata, 4);
            }
            else
            {
                std::printf("Server read failed\n");
                co_return false;
            }
        }
    }

    task<bool> udp_client()
    {
        auto endpoint = cppio::network::endpoint::parse("[::]");
        if (!endpoint)
            co_return false;

        auto to_endpoint = cppio::network::endpoint::parse("[::1]:12346");
        if (!to_endpoint)
            co_return false;

        auto client_res = cppio::network::udp_socket::bind(endpoint.value());
        if (!client_res)
            co_return false;

        auto client = std::move(client_res.value());

        while (true)
        {
            co_await cppio::sleep(std::chrono::seconds{ 1 });

            const char* data = "hi!";
            auto write_res = co_await client.write(to_endpoint.value(), data, 3);

            cppio::network::endpoint from;
            char buf[1025];
            auto read_res = co_await client.read(from, buf, 1024);

            if (read_res)
            {
                buf[read_res.value()] = 0;
                std::printf("Server replied %s\n", buf);
            }
            else
                std::printf("Read from server failed\n");
        }
    }
}

int main()
{
    // This must be called before any other cppio call, here we start 4 background workers.
    if (!cppio::initialize(0))
        return -1;

    // sadly main can't be a coroutine so we spawn this that will server as our coroutine entry
    // note that you can spawn multiple coroutines from anywhere without waiting.
    cppio::spawn(http_test());

    // Run a client test
    //cppio::spawn(client_test());

    // Host a udp server
    //cppio::spawn(udp_server());

    // And run a simple udp client that sends a message every second
    //cppio::spawn(udp_client());

    // Make sure all tasks complete, this is a work stealing wait, it will process tasks.
    cppio::wait_for_all();

    return 0;
}
