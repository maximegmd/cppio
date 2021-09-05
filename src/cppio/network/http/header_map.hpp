#pragma once

#include <map>
#include <vector>

namespace cppio::network::http
{
    struct header_map
    {
        static outcome::result<header_map> create(std::string_view text) noexcept;

        header_map() = default;
        header_map(const header_map& rhs) noexcept;
        header_map(header_map&& rhs) noexcept;
        header_map& operator=(const header_map& rhs) noexcept;
        header_map& operator=(header_map&& rhs) noexcept;

        void add(std::string key, std::string value) noexcept;
        std::vector<std::string> get(const std::string& key) const noexcept;

        std::string serialize() const noexcept;

    private:

        std::multimap<std::string, std::string> m_headers;
    };
}

#define CPPIO_NETWORK_HTTP_HEADER_MAP_INL_DO
#include <cppio/network/http/header_map.inl>
#undef CPPIO_NETWORK_HTTP_HEADER_MAP_INL_DO