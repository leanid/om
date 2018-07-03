#include "scanner.hxx"
#include <algorithm>
#include <assert.h>
#include <experimental/filesystem>
#include <iostream>
#include <list>
#include <vector>

/* TODO Locale dependency. We have to use std::wstring, std::wcout
 * to handle paths with russian names. Otherwise scanner won't find
 * any entries;
 */
/* TODO Failed skip directories with access denied. Even with
 * fs::directory_options::skip_permission_denied option scanner tries to
 * iterate over it and throws iterator error.
 */
/* TODO Add scanner's destructor that will free all pointers (directories,
 * files, etc.)
 */
/* TODO Add file size solution, add get_file_size method.
 */
/* TODO Add exceptions and replace assets.
 */

namespace fs = std::experimental::filesystem;

namespace om
{

class file;

class directory
{
public:
    directory(std::string);
    directory(){};
    directory(std::string, directory*);
    std::list<directory*> child_folders{};
    std::list<file*>      child_files{};
    directory*            parent = nullptr;
    std::string           name{};

private:
};

class file
{
public:
    size_t      size = 0;
    std::string name;
    std::string extension;
    directory*  parent = nullptr;

private:
};

class scanner::impl
{
public:
    void                        init();
    void                        init(std::string);
    void                        tell_directory_info(std::string);
    std::vector<om::directory*> dirs;
    std::vector<om::file*>      files;
    om::directory               root;
    void                        scan();

    bool single_folder_scan(om::directory*);
    bool single_folder_scan(om::directory*, std::vector<om::directory*>&,
                            std::vector<file*>&); // for futher use
    bool is_initialized{ false };
    bool root_scanned{ false };

    void                      scan(std::string);
    size_t                    total_files_count{ 0 };
    size_t                    total_files_size{ 0 };
    std::chrono::milliseconds scan_time{ 0 };
};

unsigned int file_list::size()
{
    return 0;
}

bool file_list::empty()
{
    return false;
}

void file_list::push() {}

scanner::scanner(const std::string& path_)
    : pImpl(new scanner::impl())
{
    if (path_.empty())
    {
        pImpl->root.name = fs::current_path().string();
    }
    else
    {
        fs::path path(path_);
        assert(fs::exists(path));
        if (path.is_absolute())
        {
            pImpl->root.name = path.string();
        }
        else
        {
            pImpl->root.name = (fs::current_path() / path).string();
        }
    }
    pImpl->is_initialized = true;
    pImpl->scan();
}
scanner::~scanner()
{
    delete pImpl;
    pImpl = nullptr;
}

scanner_report scanner::getReport()
{
    scanner_report result;
    result.scan_time      = pImpl->scan_time.count();
    result.total_files    = pImpl->files.size();
    result.total_folders  = pImpl->dirs.size();
    result.is_initialized = pImpl->is_initialized;
    result.scan_perfomed  = pImpl->root_scanned;
    return result;
}

fs::path get_path_from_dir(const directory* dir)
{
    fs::path path;
    while (dir->parent != nullptr)
    {
        path = dir->name / path;
        dir  = dir->parent;
    }
    return path;
}

bool scanner::impl::single_folder_scan(om::directory* dir)
{
    fs::path path = root.name / get_path_from_dir(dir);
    for (auto& p : fs::directory_iterator(
             path, fs::directory_options::skip_permission_denied))
    {
        if (fs::is_directory(p))
        {
            om::directory* tmp = new om::directory;
            tmp->name          = p.path().filename().string();
            tmp->parent        = dir;
            dir->child_folders.push_back(tmp);
            dirs.push_back(tmp);
        }
        else if (fs::is_regular_file(p))
        {
            file* tmp      = new file;
            tmp->name      = p.path().filename().string();
            tmp->parent    = dir;
            tmp->extension = p.path().extension().string();
            tmp->size      = fs::file_size(p);
            dir->child_files.push_back(tmp);
            files.push_back(tmp);
        }
    }
    return true;
}

void scanner::impl::scan()
{
    assert(is_initialized);
    std::chrono::time_point<std::chrono::system_clock> start, finish;
    start = std::chrono::system_clock::now();
    std::cout << root.name << std::endl;
    single_folder_scan(&root);

    unsigned int index_offset       = 0;
    unsigned int index_last_element = dirs.size();
    while (1)
    {
        for (unsigned int i = index_offset; i < index_last_element; ++i)
        {
            single_folder_scan(dirs[i]);
        }
        if (index_last_element == dirs.size())
            break;
        index_offset       = index_last_element;
        index_last_element = dirs.size();
    }
    finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Scan time: " << scan_time.count() << " ms." << std::endl;

    start = std::chrono::system_clock::now();
    std::sort(dirs.begin(), dirs.end(), [](om::directory* a, om::directory* b) {
        return a->name < b->name;
    });
    std::sort(files.begin(), files.end(),
              [](file* a, file* b) { return a->name < b->name; });
    std::cout << "Folders total " << dirs.size() << std::endl;
    std::cout << "Files total " << files.size() << std::endl;

    finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Sort time: " << scan_time.count() << " ms" << std::endl;
    root_scanned = true;
    return;
}

void scanner::impl::tell_directory_info(std::string name)
{
    assert(root_scanned);
    auto          a_start = std::chrono::high_resolution_clock::now();
    om::directory test_dir;
    test_dir.name = name;
    auto bounds   = std::equal_range(
        dirs.begin(), dirs.end(), &test_dir,
        [](om::directory* a, om::directory* b) { return a->name < b->name; });
    auto a_finish = std::chrono::high_resolution_clock::now();
    auto s_time   = std::chrono::duration_cast<std::chrono::nanoseconds>(
        a_finish - a_start);
    std::cout << "Search time: " << s_time.count() << std::endl;
    auto range = bounds.second - bounds.first;
    if (range != 0)
    {
        std::cout << "Total found " << range << " entries" << std::endl;
        for (auto i = bounds.first; i != bounds.second; ++i)
        {
            std::cout << "In folder "
                      << root.name / get_path_from_dir(dirs[i - dirs.begin()])
                      << std::endl;
            std::cout << "There are: "
                      << dirs[i - dirs.begin()]->child_folders.size()
                      << " folders and "
                      << dirs[i - dirs.begin()]->child_files.size() << " files."
                      << std::endl;
            std::cout << "Folders are: " << std::endl;
            for (auto p : dirs[i - dirs.begin()]->child_folders)
            {
                std::cout << "- " << p->name << std::endl;
            }
            std::cout << "Files are: " << std::endl;
            for (auto p : dirs[i - dirs.begin()]->child_files)
            {
                std::cout << "- " << p->name << std::endl;
            }
        }
    }
    else
    {
        std::cout << "No entries found on request" << std::endl;
    }
    std::cout << std::endl;
    return;
}

unsigned int scanner::get_file_size(std::string name)
{
    name = "";
    return 0;
}

bool scanner::is_file_exists(std::string name)
{
    name = "";
    return 0;
}

const std::string scanner::get_file_path(std::string name)
{
    name = "";
    return "";
}

file_list scanner::get_all_files_with_extension(std::string extn,
                                                std::string path)
{
    extn = "";
    path = "";
    file_list result;
    return result;
}

file_list scanner::get_all_files_with_name(std::string name, std::string path)
{
    name = "";
    path = "";
    file_list result;
    return result;
}

} // namespace om
