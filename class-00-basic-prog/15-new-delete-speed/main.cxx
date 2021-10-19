#include <chrono>
#include <filesystem>
#include <iostream>
#include <limits>
#include <map>

class timer_t
{
private:
    // Type aliases to make accessing nested type easier
    using clock_type  = std::chrono::high_resolution_clock;
    using second_type = std::chrono::duration<double, std::ratio<1>>;

    std::chrono::time_point<clock_type> m_beg;

public:
    timer_t()
        : m_beg{ clock_type::now() }
    {
    }

    void reset() { m_beg = clock_type::now(); }

    double elapsed() const
    {
        using namespace std::chrono;
        const auto diff = clock_type::now() - m_beg;
        return duration_cast<second_type>(diff).count();
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    using namespace std;

    auto alloc_byte = []() -> byte* { return new byte(); };

    map<double, size_t> timings;

    auto mesure_func = [](auto func) {
        timer_t                timer;
        [[maybe_unused]] byte* ptr     = func();
        double                 seconds = timer.elapsed();
        return seconds;
    };

    constexpr size_t iteration_count = 10'000'000;

    for (size_t i = 0; i < iteration_count; ++i)
    {
        double current_timing = mesure_func(alloc_byte);
        timings[current_timing] += 1;
    }

    std::cout << "print timings: distinct count: " << timings.size()
              << std::endl;

    // print double with maximum precision
    std::cout.precision(std::numeric_limits<double>::max_digits10);

    for (auto const& entry : timings)
    {
        std::cout << std::fixed << entry.first << " " << entry.second << '\n';
    }
    return 0;
}
