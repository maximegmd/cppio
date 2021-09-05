#ifndef CPPIO_NETWORK_HTTP_HEADER_MAP_INL_DO
#error Do not include this file directly!
#endif

#include <cppio/network/http/http.hpp>
#include <cppio/network/http/header_map.hpp>

namespace cppio::network::http
{
    inline outcome::result<header_map> header_map::create(std::string_view text) noexcept
    {
        header_map map;
        
        while (!text.empty())
        {
            auto pos = text.find(LINE_END);
            auto current_view = text.substr(0, pos);

            auto header_pos = current_view.find_first_of(':');
            if (header_pos == std::string_view::npos)
                return std::errc::invalid_argument;

            map.add(std::string(current_view.substr(0, header_pos)), std::string(current_view.substr(header_pos + 2)));

            text = text.substr(pos + 2);

            if (pos == std::string_view::npos)
                break;
        }

        return map;
    }

    inline header_map::header_map(const header_map& rhs) noexcept
        : m_headers(rhs.m_headers)
    {
    }

    inline header_map::header_map(header_map&& rhs) noexcept
        : m_headers(std::exchange(rhs.m_headers, {}))
    {
    }

    inline header_map& header_map::operator=(const header_map& rhs) noexcept
    {
        m_headers = rhs.m_headers;
        return *this;
    }

    inline header_map& header_map::operator=(header_map&& rhs) noexcept
    {
        std::swap(m_headers, rhs.m_headers);
        return *this;
    }

    inline void header_map::add(std::string key, std::string value) noexcept
    {
        m_headers.emplace(std::move(key), std::move(value));
    }

    inline std::vector<std::string> header_map::get(const std::string& key) const noexcept
    {
        std::vector<std::string> out;
        for (auto& [c_key, value] : m_headers)
        {
            if (c_key == key)
                out.push_back(value);
        }

        return out;
    }

    inline std::string header_map::serialize() const noexcept
    {
        std::string out;
        for (auto& [key, value] : m_headers)
        {
            out += key + ": " + value + std::string(LINE_END);
        }

        return out;
    }
}