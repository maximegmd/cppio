#pragma once

#include <cppio/platform.hpp>

namespace cppio::network::ip
{
    struct v4
    {
        static outcome::result<v4> parse(const std::string& address);

        v4() = default;
        v4(uint32_t address);
        v4(const v4&) = default;
        v4(v4&&) = default;
        v4& operator=(const v4&) = default;
        v4& operator=(v4&&) = default;

        void fill(sockaddr_storage& storage) const noexcept;

        std::string to_string() const noexcept;

    private:

        uint32_t m_address{ 0 };
    };

    struct v6
    {
        static outcome::result<v6> parse(const std::string& address);

        v6() = default;
        v6(const uint8_t address[16]);
        v6(const v6&) = default;
        v6(v6&&) = default;
        v6& operator=(const v6&) = default;
        v6& operator=(v6&&) = default;

        void fill(sockaddr_storage& storage) const noexcept;

        std::string to_string() const noexcept;

    private:

        union
        {
            uint8_t byte[16];
            uint16_t word[8];
        } m_address{};
    };
}

namespace std
{
    std::string to_string(const cppio::network::ip::v4& address) noexcept;
    std::string to_string(const cppio::network::ip::v6& address) noexcept;
}