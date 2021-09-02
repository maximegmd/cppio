#pragma once

#include <system_error>

namespace cppio
{
    enum class socket_error_code
    {
        Success = 0,
        Closed = 1,
        SystemError = 2,
        Unknown
    };
}

namespace std
{
    template <> struct is_error_code_enum<cppio::socket_error_code> : std::true_type
    {
    };
}

namespace cppio::detail
{
    class socket_error_code_category : public std::error_category
    {
    public:

        virtual const char *name() const noexcept override final { return "socket_error_code"; }
        virtual std::string message(int c) const override final
        {
            switch (static_cast<socket_error_code>(c))
            {
            case socket_error_code::Success:
                return "operation successful";
            case socket_error_code::Closed:
                return "socket is closed";
            default:
                return "unknown";
            }
        }

        virtual std::error_condition default_error_condition(int c) const noexcept override final
        {
            switch (static_cast<socket_error_code>(c))
            {
            case socket_error_code::Closed:
                return make_error_condition(std::errc::broken_pipe);
            default:
                return std::error_condition(c, *this);
            }
        }
    };

    struct socket_error_code_category_holder
    {
        inline static socket_error_code_category instance;
    };
}

namespace cppio
{
    inline std::error_code make_error_code(cppio::socket_error_code e)
    {
        return { static_cast<int>(e), cppio::detail::socket_error_code_category_holder::instance };
    }
}