#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <thread>
#if __has_include(< syncstream >)
#include <syncstream>
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    using namespace std;
    const char* tmp_file = "tmp_file.txt";
    // NOLINTNEXTLINE
    const string_view print_patterns[]  = { "012\n", "abcd\n", "|||||\n" };
    const size_t      num_of_iterations = 1024;

    ofstream out_file;
    out_file.exceptions(ios::badbit | ios::failbit);
    out_file.open(tmp_file, ios::binary | ios::trunc);

    auto print_chars_unsafe =
        [&out_file](const string_view& str, size_t num_of_iter)
    {
        while (num_of_iter--)
        {
            out_file << str;
            // fix: c++20
            // std::osyncstream(out_file) << str;
        }
    };

    array<thread, std::size(print_patterns)> threads;

    std::ranges::transform(
        print_patterns,

        begin(threads),
        [&print_chars_unsafe, &num_of_iterations](const string_view& pattern)
        { return thread{ print_chars_unsafe, pattern, num_of_iterations }; });

    std::ranges::for_each(threads, [](thread& t) { t.join(); });

    out_file.close();

    const size_t expected_size_of_file =
        std::accumulate(begin(print_patterns),
                        end(print_patterns),
                        size_t{ 0 },
                        [](size_t value, const string_view& right)
                        {
                            const size_t num_of_chars_for_pattern =
                                right.size() * num_of_iterations;
                            return value + num_of_chars_for_pattern;
                        });

    ifstream in_file;
    in_file.open(tmp_file, ios::binary | ios::ate);
    const size_t file_size = in_file.tellg();

    if (file_size == expected_size_of_file)
    {
        cout << "file_size(" << file_size << ") == expected_size_of_file("
             << expected_size_of_file << ")";
    }
    else
    {
        cout << "file_size(" << file_size << ") != expected_size_of_file("
             << expected_size_of_file << ")";
    }
    cout << endl;
    return 0;
}
