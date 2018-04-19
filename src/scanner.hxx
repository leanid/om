#include <cstddef>
#include <string>
#include <string_view>

// TODO finish this header

namespace om
{
struct file_info
{
    size_t size   = 0;
    bool   exists = false;
};

class directory_tree
{
public:
    std::string root;
    file_info   get_file_info(std::string_view file_path);
};

class directory_info
{
public:
    directory_tree tree;
    size_t         total_files_count = 0;
    size_t         total_files_size  = 0;
    seconds        scan_time         = 0;
};
} // end namespace om
