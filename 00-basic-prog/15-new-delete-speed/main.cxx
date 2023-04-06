#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <string_view>
#include <utility>

class timer
{
private:
    // Type aliases to make accessing nested type easier
    using clock_type  = std::chrono::high_resolution_clock;
    using second_type = std::chrono::duration<double, std::ratio<1>>;

    std::chrono::time_point<clock_type> m_beg;

public:
    timer()
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

template <class Func, class ArgType, class RetType> struct measure
{
    static std::pair<double, RetType> call(Func func, ArgType num_bytes)
    {
        timer   timer;
        RetType ptr     = func(num_bytes);
        double  seconds = timer.elapsed();
        return std::pair{ seconds, ptr };
    }
};

void print_timings_key_val(const std::map<double, size_t>& timings)
{
    for (auto const& entry : timings)
    {
        std::cout << std::fixed << entry.first << " " << entry.second << '\n';
    }
}

void print_max_elements(const std::string_view          prefix,
                        const std::map<double, size_t>& timings)
{
    const auto it = max_element(begin(timings),
                                end(timings),
                                [](const auto& left, const auto& right)
                                {
                                    // find most common case
                                    return left.second < right.second;
                                });

    if (it != end(timings))
    {
        std::cout << prefix << " min: " << it->first << " " << it->second
                  << '\n';
    }

    const auto it2 = max_element(begin(timings),
                                 end(timings),
                                 [](const auto& left, const auto& right)
                                 {
                                     // find most common case
                                     return left.first < right.first;
                                 });

    if (it2 != end(timings))
    {
        std::cout << prefix << " max: " << it2->first << " " << it2->second
                  << '\n';
    }
}

double accumulate_total_time(const std::map<double, size_t>& timings)
{
    return accumulate(begin(timings),
                      end(timings),
                      0.0,
                      [](double accum, const auto& entry)
                      { return accum + entry.first; });
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    using namespace std;

    auto alloc_bytes = [](size_t num_bytes) -> byte*
    { return new byte[num_bytes]; };

    auto dealloc_bytes = [](byte* ptr) -> void*
    {
        delete[] ptr;
        return nullptr;
    };

    map<double, size_t> timings_new;
    map<double, size_t> timings_delete;

    constexpr size_t iteration_count = 10'000'000;
    constexpr size_t num_bytes       = 128;

    vector<byte*> pointers(iteration_count);

    for (size_t i = 0; i < iteration_count; ++i)
    {
        auto [current_timing_new, ptr] =
            measure<decltype(alloc_bytes), size_t, byte*>::call(alloc_bytes,
                                                                num_bytes);

        timings_new[current_timing_new] += 1;

        // delete some time leter
        pointers[i] = ptr;
    }

    // try to delete in reverse order or forward? On Win10 forward faster
    for (size_t i = iteration_count - 1; i < iteration_count; --i)
    // for (size_t i = 0; i < iteration_count; ++i)
    {
        byte* ptr = pointers[i];

        [[maybe_unused]] auto [current_timing_del, _] =
            measure<decltype(dealloc_bytes), byte*, void*>::call(dealloc_bytes,
                                                                 ptr);
        timings_delete[current_timing_del] += 1;
    }

    cout << "print new timings: distinct count: " << timings_new.size() << endl;

    // print double with maximum precision
    cout.precision(numeric_limits<double>::max_digits10);

    print_timings_key_val(timings_new);
    cout << string(80, '-') << '\n';
    cout << "print delete timings: distinct count: " << timings_delete.size()
         << endl;
    print_timings_key_val(timings_delete);

    print_max_elements("element new ", timings_new);
    print_max_elements("element del ", timings_delete);

    cout << "total time for new: " << accumulate_total_time(timings_new)
         << " seconds\n";
    cout << "total time for del: " << accumulate_total_time(timings_delete)
         << " seconds\n";

    return 0;
}
