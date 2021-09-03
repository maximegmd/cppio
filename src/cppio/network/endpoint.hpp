#pragma once

#include <variant>
#include <cppio/network/ip.hpp>

namespace cppio::network
{
	struct endpoint
	{
		static outcome::result<endpoint> parse(std::string_view address) noexcept;

		endpoint(const ip::v4& rhs, uint16_t port) noexcept;
		endpoint(const ip::v6& rhs, uint16_t port) noexcept;

		uint16_t port() const noexcept { return m_port; }
		uint32_t type() const noexcept;

		void set_port(uint16_t port) noexcept;
		void set_address(const ip::v4& rhs) noexcept;
		void set_address(const ip::v6& rhs) noexcept;

		void fill(sockaddr_storage& storage) const noexcept;

		std::string to_string() const noexcept;

		endpoint() = delete;
		endpoint(const endpoint&) = default;
		endpoint(endpoint&&) = default;
		endpoint& operator=(const endpoint&) = default;
		endpoint& operator=(endpoint&&) = default;

	private:

		std::variant<ip::v4, ip::v6> m_addr;
		uint16_t m_port;
	};
}

namespace std
{
	std::string to_string(const cppio::network::endpoint& endpoint) noexcept;
}