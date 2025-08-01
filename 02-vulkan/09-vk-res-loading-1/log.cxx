#include "log.hxx"

namespace om::detail
{
struct null_buffer final : std::streambuf
{
    int overflow(int c) final { return c; }
} null;
} // namespace om::detail

namespace om
{
/// Default is null-logger
std::ostream cout(&detail::null);
} // namespace om
