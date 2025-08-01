#include "log.hxx"

namespace om
{
struct null_buffer final : std::streambuf
{
    int overflow(int c) final { return c; }
} null;

/// Default is null-logger
std::ostream cout(&null);
} // namespace om
