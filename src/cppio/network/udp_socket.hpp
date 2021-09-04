#pragma once

#include <cppio/network/base_socket.hpp>

namespace cppio::network
{
	struct udp_socket : base_socket
	{
		static outcome::result<udp_socket> bind(const endpoint& local_endpoint) noexcept;

		udp_socket() noexcept = delete;
		udp_socket(udp_socket&& rhs) noexcept;
		udp_socket& operator=(udp_socket&& rhs) noexcept;
		~udp_socket() noexcept = default;

		udp_socket(socket_t socket) noexcept;

		udp_socket(const udp_socket&) = delete;
		udp_socket& operator=(const udp_socket&) = delete;

		task<outcome::result<size_t>> read(endpoint& remote_endpoint, void* p_buffer, size_t size) noexcept;
		task<outcome::result<size_t>> write(const endpoint& remote_endpoint, const void* p_buffer, size_t size) noexcept;
	};
}