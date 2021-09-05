#ifndef CPPIO_NETWORK_HTTP_REQUEST_INL_DO
#error Do not include this file directly!
#endif

#include <cppio/network/http/http.hpp>
#include <cppio/network/http/request.hpp>

#include <charconv>

using namespace std::literals;


namespace cppio::network::http
{
    inline outcome::result<request> request::create(std::string_view text) noexcept
    {
        request r;
        
        auto pos = text.find(BLOCK_END);
        auto current_view = text.substr(0, pos);

        auto http_pos = current_view.find(LINE_END);
        if (http_pos == std::string_view::npos)
            return std::errc::invalid_argument;

        auto http_line = current_view.substr(0, http_pos);
        {
            auto method_pos = http_line.find_first_of(' ');
            auto path_pos = http_line.find_first_of(' ', method_pos + 1);

            auto method = http_line.substr(0, method_pos);
            auto path = http_line.substr(method_pos + 1, path_pos - method_pos - 1);
            auto version = http_line.substr(path_pos + 1);

            r.m_method = cppio::network::http::create(std::string(method));
            r.m_path = std::string(path);
            r.m_version = std::string(version);
        }
        

        auto headers = header_map::create(current_view.substr(http_pos + 2));
        if(!headers)
            return std::errc::invalid_argument;

        r.m_headers = std::move(headers.value());

        return r;
    }

    inline request::request(const request& rhs) noexcept
        : m_method(rhs.m_method)
        , m_version(rhs.m_version)
        , m_path(rhs.m_path)
        , m_headers(rhs.m_headers)
    {
    }

    inline request::request(request&& rhs) noexcept
        : m_method(std::exchange(rhs.m_method, {}))
        , m_version(std::exchange(rhs.m_version, {}))
        , m_path(std::exchange(rhs.m_path, {}))
        , m_headers(std::exchange(rhs.m_headers, {}))
    {
    }

    inline request& request::operator=(const request& rhs) noexcept
    {
        m_method = rhs.m_method;
        m_version = rhs.m_version;
        m_path = rhs.m_path;
        m_headers = rhs.m_headers;
        return *this;
    }

    inline request& request::operator=(request&& rhs) noexcept
    {
        std::swap(m_method, rhs.m_method);
        std::swap(m_version, rhs.m_version);
        std::swap(m_path, rhs.m_path);
        std::swap(m_headers, rhs.m_headers);
        return *this;
    }

    inline method request::get_method() const noexcept
    {
        return m_method;
    }

    inline const std::string& request::get_path() const noexcept
    {
        return m_path;
    }

    inline const std::string& request::get_version() const noexcept
    {
        return m_version;
    }

    inline void request::set_method(method m) noexcept
    {
        m_method = m;
    }

    inline void request::set_path(const std::string& path) noexcept
    {
        m_path = path;
    }

    inline void request::set_version(const std::string& version) noexcept
    {
        m_version = version;
    }

    inline std::string request::serialize() const noexcept
    {
        std::string out;
        out += to_string(m_method) + " " + m_path + " " + m_version + std::string(LINE_END);
        out += m_headers.serialize();
        out += std::string(LINE_END);
        // content

        return out;
    }
}