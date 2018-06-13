#include <chrono>
#include <cstddef>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// TODO finish this header

namespace om
{

using milliseconds = std::chrono::milliseconds;

class file;

class directory
{
public:
    directory(std::string);
    directory(){};
    std::list<directory*> child_folders{};
    std::list<file*>      child_files{};
    directory*            parent = nullptr;
    std::string           name{};

private:
};

class file
{
public:
    size_t      get_size();
    size_t      size = 0;
    std::string name;
    directory*  parent = nullptr;

private:
};

class scanner
{
public:
    scanner(){};
    void init();
    void init(std::string);
    void tell_directory_info(std::string);
    void scan();

protected:
private:
    om::directory               root;
    std::vector<om::directory*> dirs;
    std::vector<om::file*>      files;
    bool                        single_folder_scan(om::directory*);
    bool single_folder_scan(om::directory*, std::vector<om::directory*>&,
                            std::vector<om::file*>&); // for futher use
    bool is_initialized{ false };
    bool root_scanned{ false };

    void         scan(std::string);
    size_t       total_files_count{ 0 };
    size_t       total_files_size{ 0 };
    milliseconds scan_time{ 0 };
};

} // end namespace om
