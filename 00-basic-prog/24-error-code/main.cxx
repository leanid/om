#include <iostream>
#include <stdexcept>
#include <system_error>
#include <type_traits>

namespace om
{
enum class custom_errc
{
    some_strange_error,
    other_error_we_got,
    new_one_error,
    more_error_kind
};
}

namespace std
{

/// this code marks 'om::custom_errc' can be automatically converted to
/// std::error_code or std::error_condition
template <> struct is_error_code_enum<om::custom_errc> : public true_type
{
};

std::error_code make_error_code(om::custom_errc err) noexcept
{
    class custom_category : public error_category
    {
    public:
        const char* name() const noexcept override { return "custom"; }
        string      message(int err) const override
        {
            auto code = static_cast<om::custom_errc>(err);
            switch (code)
            {
                case om::custom_errc::some_strange_error:
                    return "some strange error";
                case om::custom_errc::other_error_we_got:
                    return "other error we got";
                case om::custom_errc::new_one_error:
                    return "new one error";
                case om::custom_errc::more_error_kind:
                    return "more error kind";
            }
            throw std::invalid_argument("error: now such enum value");
        };
        error_condition default_error_condition(int i) const noexcept override
        {
            if (static_cast<om::custom_errc>(i) ==
                om::custom_errc::some_strange_error)
            {
                return make_error_condition(static_cast<errc>(i));
            }
            return error_category::default_error_condition(i);
        }
    };

    static custom_category category{};

    return std::error_code(static_cast<int>(err), category);
}

} // namespace std

int main(int argc, char** argv)
{
    std::cout << "1. example how to make your custom error codes with custom "
                 "error_category:\n";

    std::error_code code =
        std::make_error_code(om::custom_errc::some_strange_error);
    std::cout << " custom error_code code = " << code
              << " message:" << code.message() << std::endl;
    std::cout << " default error condition category name: "
              << code.default_error_condition().category().name() << std::endl;
    std::cout << " default error condition message: "
              << code.default_error_condition().message() << std::endl;

    std::cout << "2. example posix errno codes\n";

    std::cout << "int value | ec.category | ec.message | ec.condition.category "
                 "| ec.condition.message |"
              << std::endl;

    for (unsigned i = 0; i < 255u; i++)
    {
        auto            error     = static_cast<std::errc>(i);
        std::error_code ec        = std::make_error_code(error);
        auto            msg       = ec.message();
        auto            category  = ec.category().name();
        auto            condition = ec.default_error_condition();
        auto            category2 = condition.category().name();
        auto            msg2      = condition.message();
        std::cout << i << ": " << category << ' ' << msg << " " << category2
                  << " " << msg2 << std::endl;
    }

    return std::cout.fail();
}
