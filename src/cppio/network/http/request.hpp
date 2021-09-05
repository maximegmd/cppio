#pragma once

#include <cppio/network/http/header_map.hpp>
#include <cppio/network/http/method.hpp>

namespace cppio::network::http
{
    struct request
    {
        static outcome::result<request> create(std::string_view text) noexcept;

        request() = default;
        request(const request& rhs) noexcept;
        request(request&& rhs) noexcept;
        request& operator=(const request& rhs) noexcept;
        request& operator=(request&& rhs) noexcept;

        cppio::network::http::method method() const noexcept;
        const std::string& path() const noexcept;
        const std::string& version() const noexcept;

        void set_method(cppio::network::http::method m) noexcept;
        void set_path(const std::string& path) noexcept;
        void set_version(const std::string& version) noexcept;

        std::string serialize() const noexcept;

    private:

        cppio::network::http::method m_method{};
        std::string m_version{"HTTP/1.1"};
        std::string m_path{};
        header_map m_headers{};
    };
}

#define CPPIO_NETWORK_HTTP_REQUEST_INL_DO
#include <cppio/network/http/request.inl>
#undef CPPIO_NETWORK_HTTP_REQUEST_INL_DO