#pragma once
#include <chrono>
#include <iostream>
#include <utility>

namespace om::tools
{
class report_duration
{
public:
    report_duration(std::ostream& log, std::string name)
        : out{ log }
        , name(std::move(name))
        , start(std::chrono::high_resolution_clock::now())
    {
    }

    ~report_duration() noexcept
    {
        try
        {
            out << name << " took: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::high_resolution_clock::now() - start)
                       .count()
                << "ms" << std::endl;
        }
        catch (...) // NOLINT
        {
            // can't throw from a destructor
        }
    }

private:
    std::ostream&                                               out;
    std::string                                                 name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
} // namespace om::tools
