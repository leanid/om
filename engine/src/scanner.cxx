#include "scanner.hxx"
#include <algorithm>
#include <assert.h>
#include <experimental/filesystem>
#include <iostream>
#include <vector>

// TODO ... Add twin filenames handling

namespace fs = std::experimental::filesystem;

om::directory_info::directory_info(std::string_view dir)
{
    fs::path path_(dir);
    if (path_.is_absolute())
    {
        path = path_.string();
    }
    else
    {
        path = (fs::current_path() / path_).string();
    }
    assert(fs::exists(path)); // Exception may be implemented
}

void om::directory_info::scan()
{
    std::chrono::time_point<std::chrono::system_clock> start, finish;
    start = std::chrono::system_clock::now();

    fs::recursive_directory_iterator begin(path);
    fs::recursive_directory_iterator end;
    std::vector<fs::path>            files_list;

    std::copy_if(begin, end, std::back_inserter(files_list),
                 [](const fs::path& p) { return fs::is_regular_file(p); });
    for (auto& p : files_list)
    {
        file_info f;
        f.path   = p.string();
        f.exists = true;
        f.size   = fs::file_size(p);
        files.insert(
            std::pair<std::string, file_info>(p.filename().string(), f));
    }
    finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    return;
    // TODO ... fill "total_files_size" and "total_files_count" members.
}

void om::directory_info::tell() // Test method. Will be removed soon
{
    for (auto& p : files)
    {
        std::cout << p.first << " size: " << p.second.size << " bytes\n";
    }
}

om::file_info om::directory_info::get_file_info(std::string_view file_path)
{
    file_info info;
    auto      it = files.find(file_path.data());
    if (it != files.end())
        return it->second;
    return info;
}

/*int main()
{
    om::directory_info folder("engine");
    folder.scan();
    folder.tell();
    std::cout << "Scan time: " << folder.scan_time.count() << " milliseconds\n";
    om::file_info nfo = folder.get_file_info("main.cxx");
    if (nfo.exists)
    {
        std::cout << "File size: " << nfo.size << '\n';
    }
    else
    {
        std::cout << "File not found" << '\n';
    }
}*/
