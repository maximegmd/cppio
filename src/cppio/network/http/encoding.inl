#ifndef CPPIO_NETWORK_HTTP_ENCODING_INL_DO
#error Do not include this file directly!
#endif

#include <cppio/network/http/http.hpp>

namespace cppio::network::http
{
    namespace details
    {
        inline char from_hex(char ch) 
        {
            return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
        }

        inline char to_hex(char code) 
        {
            static const char hex[] = "0123456789abcdef";
            return hex[code & 15];
        }
    }

    inline std::string url_encode(std::string_view url)
    {
        std::string result;
        result.reserve(url.size() * 3 + 1);

        for(auto ch : url)
        {
            if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~')
                result.push_back(ch);
            else
            {
                result.push_back('%');
                result.push_back(details::to_hex(ch >> 4));
                result.push_back(details::to_hex(ch & 15));
            }
        }
        result.push_back(0);
        
        return result;
    }

    inline std::string url_decode(std::string_view url)
    {
        std::string result;
        for (auto it = std::begin(url), end = std::end(url); it != end; ++it)
        {
            auto ch = *it;
            if (ch == '%')
            {
                auto part_one = it + 1;
                auto part_two = it + 2;
                if (part_one != end && part_two != end)
                {
                    result.push_back(details::from_hex(*part_one) << 4 | details::from_hex(*part_two));
                    it += 2;
                }
            }
            else if (ch == '+')
                result.push_back(' ');
            else
                result.push_back(ch);
        }
        return result;
    }
}