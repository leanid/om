#pragma once
import std;

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
extern std::ostream cout;
} // namespace om
