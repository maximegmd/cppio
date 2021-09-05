#pragma once

#include <cppio/network/http/header_map.hpp>
#include <cppio/network/http/method.hpp>

namespace cppio::network::http
{
    struct response
    {
        static outcome::result<response> create(std::string_view text) noexcept;

        response() = default;
        response(const response& rhs) noexcept;
        response(response&& rhs) noexcept;
        response& operator=(const response& rhs) noexcept;
        response& operator=(response&& rhs) noexcept;

        uint32_t get_status() const noexcept;
        const std::string& get_version() const noexcept;
        const std::string& get_content() const noexcept;
        header_map& get_headers() noexcept;
        const header_map& get_headers() const noexcept;

        void set_status(uint32_t code) noexcept;
        void set_version(const std::string& version) noexcept;
        void set_content(const std::string& content) noexcept;

        std::string serialize() noexcept;

    private:

        std::string m_version{"HTTP/1.1"};
        uint32_t m_code{200};
        header_map m_headers{};
        std::string m_content;
    };
}

#define CPPIO_NETWORK_HTTP_RESPONSE_INL_DO
#include <cppio/network/http/response.inl>
#undef CPPIO_NETWORK_HTTP_RESPONSE_INL_DO