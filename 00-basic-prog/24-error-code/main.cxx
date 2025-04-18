#include <algorithm>
#include <format>
#include <iostream>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string>
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
        [[nodiscard]] const char* name() const noexcept override
        {
            return "custom";
        }
        [[nodiscard]] string message(int err) const override
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
        [[nodiscard]] error_condition default_error_condition(
            int i) const noexcept override
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

    return { static_cast<int>(err), category };
}

} // namespace std
// NOLINTNEXTLINE
int main(int argc, char** argv)
{
    using namespace std;
    cout << "1. example how to make your custom error codes with custom "
            "error_category:\n";

    error_code code = make_error_code(om::custom_errc::some_strange_error);
    cout << " custom error_code code = " << code
         << " message:" << code.message() << "\n";
    cout << " default error condition category name: "
         << code.default_error_condition().category().name() << "\n";
    cout << " default error condition message: "
         << code.default_error_condition().message() << "\n";

    cout << "2. example posix errno codes:\n\n";

    struct
    {
        size_t i                  = "int"s.size();
        size_t message            = "message"s.size();
        size_t category           = "category"s.size();
        size_t condition_category = "condition category"s.size();
        size_t condition_message  = "condition message"s.size();

        size_t* begin() noexcept { return &i; }
        size_t* end() noexcept { return &condition_message + 1; }
    } max_len;

    struct table_row
    {
        string                      i_str;
        string                      message;
        string                      category;
        string                      category2;
        string                      msg2;
        [[nodiscard]] const string* begin() const noexcept { return &i_str; }
        [[nodiscard]] const string* end() const noexcept { return &msg2 + 1; }
    };

    auto to_error_code_member_strings = [](int errno_code) -> table_row
    {
        auto       error     = static_cast<errc>(errno_code);
        error_code ec        = make_error_code(error);
        auto       condition = ec.default_error_condition();

        string i_str     = std::to_string(errno_code);
        string message   = ec.message();
        string category  = ec.category().name();
        string category2 = condition.category().name();
        string msg2      = condition.message();

        return { .i_str     = i_str,
                 .message   = message,
                 .category  = category,
                 .category2 = category2,
                 .msg2      = msg2 };
    };

    const int start  = 0;
    const int finish = 133 + 1;

    auto table_rows = ranges::iota_view{ start, finish } |
                      views::transform(to_error_code_member_strings);

    auto update_max_column_sizes = [&max_len](table_row row)
    {
        auto select_max = [](auto l, auto r) { return max(l, r); };
        auto row_sizes  = row | views::transform(&string::size);
        auto max_view   = views::zip_transform(select_max, max_len, row_sizes);
        ranges::copy(max_view, max_len.begin());
    };

    ranges::for_each(table_rows, update_max_column_sizes);

    auto to_string = [&max_len](const table_row& v) -> string
    {
        string row = std::format("|{:>{}}|{:<{}}|{:<{}}|{:<{}}|{:<{}}|",
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
        return row;
    };
    string table_title = to_string(table_row{ .i_str     = "int",
                                              .message   = "message",
                                              .category  = "category",
                                              .category2 = "condition_category",
                                              .msg2 = "condition_message" });

    cout << table_title << "\n";
    cout << string(table_title.size(), '-') << "\n";

    auto rows = ranges::iota_view{ start, finish } |
                views::transform(to_error_code_member_strings) |
                views::transform(to_string);

    ranges::copy(rows, ostream_iterator<string>(cout, "\n"));

    return cout.fail();
}
