export module log;

import std;

namespace om::detail
{
struct null_buffer final : std::streambuf
{
    int overflow(int c) final { return c; }
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
