#include <cppio/network/ip.hpp>
#include <cppio/platform.hpp>

namespace cppio::network::ip
{
	outcome::result<v4> v4::parse(const std::string& address)
	{
		in_addr addr;
		if (inet_pton(AF_INET, address.c_str(), &addr) == 1)
		{
			return v4{ ntohl(addr.s_addr) };
		}

		return network_error_code::BadFormat;
	}

	v4::v4(uint32_t address)
		: m_address(address)
	{
	}

	void v4::fill(sockaddr_storage& storage) const noexcept
	{
		storage.ss_family = AF_INET;
		((sockaddr_in*)&storage)->sin_addr.s_addr = htonl(m_address);
	}

	std::string v4::to_string() const noexcept
	{
		std::ostringstream oss;

		oss << ((m_address >> 24) & 0xFF) << "."
			<< ((m_address >> 16) & 0xFF) << "."
			<< ((m_address >> 8) & 0xFF) << "."
			<< ((m_address >> 0) & 0xFF);

		return oss.str();
	}

	outcome::result<v6> v6::parse(const std::string& address)
	{
		in6_addr addr;
		if (inet_pton(AF_INET6, address.c_str(), &addr) == 1)
		{
			return v6{ addr.s6_addr };
		}

		return network_error_code::BadFormat;
	}

	v6::v6(uint8_t address[16])
	{
		for (auto i = 0; i < std::size(m_address.word); ++i)
		{
			uint16_t word = ntohs(*reinterpret_cast<uint16_t*>(address + i * 2));
			m_address.word[i] = word;
		}
	}

	void v6::fill(sockaddr_storage& storage) const noexcept
	{
		storage.ss_family = AF_INET6;

		for (auto i = 0; i < std::size(m_address.word); ++i)
		{
			auto* loc = &((sockaddr_in6*)&storage)->sin6_addr.s6_addr[i * 2];

			*(uint16_t*)loc = htons(m_address.word[i]);
		}	
	}

	std::string v6::to_string() const noexcept
	{
		std::ostringstream oss;

		oss << "[" << std::hex << m_address.word[0] << ":"
			<< m_address.word[1] << ":"
			<< m_address.word[2] << ":"
			<< m_address.word[3] << ":"
			<< m_address.word[4] << ":"
			<< m_address.word[5] << ":"
			<< m_address.word[6] << ":"
			<< m_address.word[7] << "]";

		return oss.str();
	}
}

std::string std::to_string(const cppio::network::ip::v4& address) noexcept
{
	return std::string();
}

std::string std::to_string(const cppio::network::ip::v6& address) noexcept
{
	return std::string();
}
