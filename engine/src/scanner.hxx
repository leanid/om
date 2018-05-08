#include <chrono>
#include <cstddef>
#include <map>
#include <string>
#include <string_view>

// TODO finish this header

using milliseconds = std::chrono::milliseconds;

namespace om
{
struct file_info
{
    std::string path;
    size_t      size   = 0;
    bool        exists = false;
};

class directory_info
{
private:
    std::multimap<std::string, file_info> files;
    std::string                           path;

public:
    directory_info(std::string_view);
    size_t       total_files_count{ 0 };
    size_t       total_files_size{ 0 };
    milliseconds scan_time{ 0 };

    file_info get_file_info(std::string_view file_path);
    void      scan();
    void      tell(); // test function. will be removed soon
};
} // end namespace om
