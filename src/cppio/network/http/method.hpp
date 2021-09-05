#pragma once

#include <cppio/platform.hpp>

#undef DELETE

namespace cppio::network::http
{
    enum class method
    {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATCH
    };

    std::string to_string(method m) noexcept
    {
        switch (m)
        {
        case method::GET:
            return "GET";
        case method::HEAD:
            return "HEAD";
        case method::POST:
            return "POST";
        case method::PUT:
            return "PUT";
        case method::DELETE:
            return "DELETE";
        case method::TRACE:
            return "TRACE";
        case method::OPTIONS:
            return "OPTIONS";
        case method::CONNECT:
            return "CONNECT";
        case method::PATCH:
            return "PATCH";
        }
        return "";
    }

    method create(const std::string& method) noexcept
    {
        if (method == to_string(method::GET))
        {
            return method::GET;
        }
        else if (method == to_string(method::HEAD))
        {
            return method::HEAD;
        }
        else if (method == to_string(method::POST))
        {
            return method::POST;
        }
        else if (method == to_string(method::PUT))
        {
            return method::PUT;
        }
        else if (method == to_string(method::DELETE))
        {
            return method::DELETE;
        }
        else if (method == to_string(method::TRACE))
        {
            return method::TRACE;
        }
        else if (method == to_string(method::OPTIONS))
        {
            return method::OPTIONS;
        }
        else if (method == to_string(method::CONNECT))
        {
            return method::CONNECT;
        }
        else if (method == to_string(method::PATCH))
        {
            return method::PATCH;
        }

        return method::GET;
    }
}