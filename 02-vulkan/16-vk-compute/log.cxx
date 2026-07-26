export module log;

import std;

namespace om::detail
{
struct null_buffer final : std::streambuf
{
    using traits   = std::char_traits<char>;
    using char_int = traits::int_type;
    /// @brief consume every char and pretend everything is Ok.
    /// @see: https://en.cppreference.com/w/cpp/io/basic_streambuf/overflow
    char_int overflow(char_int ch) final
    {
        if (traits::eq_int_type(ch, traits::eof()))
        {
            return traits::not_eof(ch);
        }
        return ch;
    }
} null;
} // namespace om::detail

namespace om
{
/// global logger
/// default is null_logger
/// how to enable:
/// ```cpp
///     if (verbose)
///     {
///         om::cout.rdbuf(std::clog.rdbuf());
///     }
/// ```
export std::ostream cout(&detail::null);
} // namespace om
