#ifndef CPPIO_NETWORK_HTTP_RESPONSE_INL_DO
#error Do not include this file directly!
#endif

#include <cppio/network/http/http.hpp>
#include <cppio/network/http/request.hpp>

#include <charconv>
#include "response.hpp"

using namespace std::literals;


namespace cppio::network::http
{
    inline outcome::result<response> response::create(std::string_view text) noexcept
    {
        response r;
        
        auto pos = text.find(BLOCK_END);
        auto current_view = text.substr(0, pos);

        auto http_pos = current_view.find(LINE_END);
        if (http_pos == std::string_view::npos)
            return std::errc::invalid_argument;

        auto http_line = current_view.substr(0, http_pos);
        {
            auto version_pos = http_line.find_first_of(' ');
            auto code_pos = http_line.find_first_of(' ', version_pos + 1);

            auto version = http_line.substr(0, version_pos);
            auto code = http_line.substr(version_pos + 1, code_pos - version_pos - 1);
            auto status = http_line.substr(code_pos + 1);

            r.m_version = std::string(version);
            r.m_code = std::stoul(std::string(code));
        }
        
        auto headers = header_map::create(current_view.substr(http_pos + 2));
        if(!headers)
            return std::errc::invalid_argument;

        r.m_headers = std::move(headers.value());

        return r;
    }

    inline response::response(const response& rhs) noexcept
        : m_version(rhs.m_version)
        , m_code(rhs.m_code)
        , m_content(rhs.m_content)
        , m_headers(rhs.m_headers)
    {
    }

    inline response::response(response&& rhs) noexcept
        : m_version(std::exchange(rhs.m_version, {}))
        , m_code(std::exchange(rhs.m_code, {}))
        , m_content(std::exchange(rhs.m_content, {}))
        , m_headers(std::exchange(rhs.m_headers, {}))
    {
    }

    inline response& response::operator=(const response& rhs) noexcept
    {
        m_code = rhs.m_code;
        m_version = rhs.m_version;
        m_content = rhs.m_content;
        m_headers = rhs.m_headers;
        return *this;
    }

    inline response& response::operator=(response&& rhs) noexcept
    {
        std::swap(m_code, rhs.m_code);
        std::swap(m_version, rhs.m_version);
        std::swap(m_content, rhs.m_content);
        std::swap(m_headers, rhs.m_headers);
        return *this;
    }

    inline uint32_t response::status() const noexcept
    {
        return m_code;
    }

    inline const std::string& response::version() const noexcept
    {
        return m_version;
    }

    inline const std::string& response::content() const noexcept
    {
        return m_content;
    }

    inline header_map& response::headers() noexcept
    {
        return m_headers;
    }

    inline const header_map& response::headers() const noexcept
    {
        return m_headers;
    }

    inline void response::set_content(const std::string& content) noexcept
    {
        m_content = content;
    }

    inline void response::set_status(uint32_t code) noexcept
    {
        m_code = code;
    }

    inline void response::set_version(const std::string& version) noexcept
    {
        m_version = version;
    }

    inline std::string response::serialize() noexcept
    {
        std::string out;

        m_headers.add("Content-Length", std::to_string(m_content.size()));

        out += version() + " " + std::to_string(status()) + " OK" + std::string(LINE_END);
        out += m_headers.serialize();
        out += std::string(LINE_END);
        out += content();
        // content

        return out;
    }
}