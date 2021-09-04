#include <cppio/network/endpoint.hpp>
#include <cppio/platform.hpp>
#include <cppio/impl/meta.hpp>
#include <limits>

#undef max

namespace cppio::network
{
	outcome::result<endpoint> endpoint::parse(std::string_view address) noexcept
	{
		auto port_separator = address.find_last_of(':');
		if (port_separator != std::string_view::npos && address[address.size() - 1] != ']')
		{
			auto port = std::stoul(address.substr(port_separator + 1).data());
			if (port <= std::numeric_limits<uint16_t>::max())
			{
				std::string ip_addr{ address.substr(0, port_separator) };

				if (ip_addr[ip_addr.size() - 1] == ']' && ip_addr[0] == '[')
				{
					ip_addr[ip_addr.size() - 1] = '\0';
					auto ip = ip::v6::parse(ip_addr.data() + 1);
					if (ip)
						return endpoint{ ip.value(), port & 0xFFFF };
				}
				else
				{
					auto ipv4 = ip::v4::parse(ip_addr.data());
					if (ipv4)
						return endpoint{ ipv4.value(), port & 0xFFFF };
				}			
			}
		}

		auto ipv6 = ip::v6::parse(std::string(address));
		if (ipv6)
			return endpoint{ ipv6.value(), 0 };

		auto ipv4 = ip::v4::parse(std::string(address));
		if (ipv4)
			return endpoint{ ipv4.value(), 0 };

		return network_error_code::BadFormat;
	}

	endpoint::endpoint()
		: m_port{ 0 }
	{

	}

	endpoint::endpoint(const sockaddr_storage& storage) noexcept
	{
		if (storage.ss_family == AF_INET)
		{
			auto* addr = (const sockaddr_in*)&storage;
			m_addr = ip::v4{ ntohl(addr->sin_addr.s_addr) };
			m_port = ntohs(addr->sin_port);
		}
		else
		{
			auto* addr = (const sockaddr_in6*)&storage;
			m_addr = ip::v6( addr->sin6_addr.s6_addr );
			m_port = ntohs(addr->sin6_port);
		}
	}

	endpoint::endpoint(const ip::v4& rhs, uint16_t port) noexcept
		: m_addr(rhs)
		, m_port(port)
	{
	}

	endpoint::endpoint(const ip::v6& rhs, uint16_t port) noexcept
		: m_addr(rhs)
		, m_port(port)
	{
	}

	uint32_t endpoint::type() const noexcept
	{
		return std::visit(impl::meta::overloaded(
			[](const ip::v4& ip) { return AF_INET; },
			[](const ip::v6& ip) { return AF_INET6; }
		), m_addr);
	}

	void endpoint::set_port(uint16_t port) noexcept
	{
		m_port = port;
	}

	void endpoint::set_address(const ip::v4& rhs) noexcept
	{
		m_addr = rhs;
	}

	void endpoint::set_address(const ip::v6& rhs) noexcept
	{
		m_addr = rhs;
	}

	void endpoint::fill(sockaddr_storage& storage) const noexcept
	{
		std::visit(impl::meta::overloaded(
			[this, &storage](const ip::v4& ip) 
			{ 
				ip.fill(storage); 
				((sockaddr_in*)&storage)->sin_port = htons(m_port);
			},
			[this, &storage](const ip::v6& ip) 
			{ 
				ip.fill(storage); 
				((sockaddr_in6*)&storage)->sin6_port = htons(m_port);
				((sockaddr_in6*)&storage)->sin6_scope_id = 0;
			}
		), m_addr);
	}

	std::string endpoint::to_string() const noexcept
	{
		std::string result = std::visit(impl::meta::overloaded(
			[](const ip::v4& ip) { return ip.to_string(); },
			[](const ip::v6& ip) { return ip.to_string(); }
		), m_addr);

		if (m_port)
			result += ":" + std::to_string(m_port);

		return result;
	}
}