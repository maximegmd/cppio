#pragma once

#include <cppio/platform.hpp>

namespace cppio::network::http
{
    constexpr static std::string_view LINE_END{ "\r\n" };
    constexpr static std::string_view BLOCK_END{ "\r\n\r\n" };
}