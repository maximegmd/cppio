#pragma once

#include <system_error>

namespace cppio
{
    enum class network_error_code
    {
        Success = 0,
        Closed = 1,
        SystemError = 2,
        BadFormat = 3,
        Unknown
    };
}

namespace std
{
    template <> struct is_error_code_enum<cppio::network_error_code> : std::true_type
    {
    };
}

namespace cppio::detail
{
    class network_error_code_category : public std::error_category
    {
    public:

        virtual const char *name() const noexcept override final { return "network_error_code"; }
        virtual std::string message(int c) const override final
        {
            switch (static_cast<network_error_code>(c))
            {
            case network_error_code::Success:
                return "operation successful";
            case network_error_code::Closed:
                return "socket is closed";
            case network_error_code::BadFormat:
                return "bad format provided";
            default:
                return "unknown";
            }
        }

        virtual std::error_condition default_error_condition(int c) const noexcept override final
        {
            switch (static_cast<network_error_code>(c))
            {
            case network_error_code::Closed:
                return make_error_condition(std::errc::broken_pipe);
            case network_error_code::BadFormat:
                return make_error_condition(std::errc::invalid_argument);
            default:
                return std::error_condition(c, *this);
            }
        }
    };

    struct network_error_code_category_holder
    {
        inline static network_error_code_category instance;
    };
}

namespace cppio
{
    inline std::error_code make_error_code(cppio::network_error_code e)
    {
        return { static_cast<int>(e), cppio::detail::network_error_code_category_holder::instance };
    }
}