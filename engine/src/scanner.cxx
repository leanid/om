#include "scanner.hxx"
#include <algorithm>
#include <assert.h>
#include <experimental/filesystem>
#include <iostream>
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

void om::scanner::init()
{
    root.name      = fs::current_path().string();
    is_initialized = true;
    return;
}

void om::scanner::init(std::string path_)
{
    fs::path path(path_);
    assert(fs::exists(path));
    if (path.is_absolute())
    {
        root.name = path.string();
    }
    else
    {
        root.name = (fs::current_path() / path).string();
    }
    is_initialized = true;
}

fs::path get_path_from_dir(const om::directory* dir)
{
    fs::path path;
    while (dir->parent != nullptr)
    {
        path = dir->name / path;
        dir  = dir->parent;
    }
    return path;
}

bool om::scanner::single_folder_scan(om::directory* dir)
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
            om::file* tmp = new om::file;
            tmp->name     = p.path().filename().string();
            tmp->parent   = dir;
            dir->child_files.push_back(tmp);
            files.push_back(tmp);
        }
    }
    return true;
}

bool om::scanner::single_folder_scan(om::directory*               dir,
                                     std::vector<om::directory*>& vec_dir,
                                     std::vector<om::file*>&      vec_file)

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
            vec_dir.push_back(tmp);
        }
        else if (fs::is_regular_file(p))
        {
            om::file* tmp = new om::file;
            tmp->name     = p.path().filename().string();
            tmp->parent   = dir;
            vec_file.push_back(tmp);
        }
    }
    return true;
}

void om::scanner::scan()
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
              [](om::file* a, om::file* b) { return a->name < b->name; });
    std::cout << "Folders total " << dirs.size() << std::endl;
    std::cout << "Files total " << files.size() << std::endl;

    finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Sort time: " << scan_time.count() << " ms" << std::endl;
    root_scanned = true;
    return;
}

void om::scanner::tell_directory_info(std::string name)
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
