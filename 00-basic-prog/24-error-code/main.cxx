#include <algorithm>
#include <format>
#include <iostream>
#include <ranges>
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
    using namespace std;
    cout << "1. example how to make your custom error codes with custom "
            "error_category:\n";

    error_code code = make_error_code(om::custom_errc::some_strange_error);
    cout << " custom error_code code = " << code
         << " message:" << code.message() << endl;
    cout << " default error condition category name: "
         << code.default_error_condition().category().name() << endl;
    cout << " default error condition message: "
         << code.default_error_condition().message() << endl;

    cout << "2. example posix errno codes\n";

    struct
    {
        size_t i                  = "int"s.size();
        size_t category           = "category"s.size();
        size_t message            = "message"s.size();
        size_t condition_category = "condition category"s.size();
        size_t condition_message  = "condition message"s.size();
    } max_len;

    struct tbl_values
    {
        string i_str;
        string message;
        string category;
        string category2;
        string msg2;
    };

    auto to_strings = [](int errno_code) -> tbl_values
    {
        auto       error     = static_cast<errc>(errno_code);
        error_code ec        = make_error_code(error);
        auto       condition = ec.default_error_condition();

        string i_str     = to_string(errno_code);
        string message   = ec.message();
        string category  = ec.category().name();
        string category2 = condition.category().name();
        string msg2      = condition.message();

        return { i_str, message, category, category2, msg2 };
    };

    const int start  = 0;
    const int finish = 133 + 1;

    for (tbl_values v :
         ranges::iota_view{ start, finish } | views::transform(to_strings))
    {
        auto& m = max_len;

        m.i                  = max(m.i, v.i_str.size());
        m.message            = max(m.message, v.message.size());
        m.category           = max(m.category, v.category.size());
        m.condition_category = max(m.condition_category, v.category2.size());
        m.condition_message  = max(m.condition_message, v.msg2.size());
    }

    auto format_line = [&max_len](const tbl_values& v) -> string
    {
        string line = std::format("|{:>{}}|{:<{}}|{:<{}}|{:<{}}|{:<{}}|",
                                  v.i_str,
                                  max_len.i,
                                  v.category,
                                  max_len.category,
                                  v.message,
                                  max_len.message,
                                  v.category2,
                                  max_len.condition_category,
                                  v.msg2,
                                  max_len.condition_message);
        return line;
    };
    string table_title =
        format_line(tbl_values{ .i_str     = "int",
                                .message   = "message",
                                .category  = "category",
                                .category2 = "condition_category",
                                .msg2      = "condition_message" });

    cout << table_title << endl;
    cout << string(table_title.size(), '-') << endl;

    for (string line : ranges::iota_view{ start, finish } |
                           views::transform(to_strings) |
                           views::transform(format_line))
    {
        cout << line << endl;
    }
    return cout.fail();
}
