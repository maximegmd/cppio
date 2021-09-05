#pragma once

#include <string>

namespace cppio::network::http
{
    std::string url_encode(std::string_view url);
    std::string url_decode(std::string_view url);
}

#define CPPIO_NETWORK_HTTP_ENCODING_INL_DO
#include <cppio/network/http/encoding.inl>
#undef CPPIO_NETWORK_HTTP_ENCODING_INL_DO