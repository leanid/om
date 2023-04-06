/**
 * Now I am working on two different scanner implementations:
 * 1) Uses dirent.h. It fast, it does less memory allocations,
 * it uses less memory. But it needs more code: manual path
 * operations (strip, concatenate, split), getting current path,
 * c-like string processing, lots of char arrays, convertings, etc.
 *
 * ALSO! There is Win32 implementation of dirent.h
 * It's available here:
 * https://github.com/tronkko/dirent/releases
 *
 * 2) std::filesystem. It is very simple and comfortable to use.
 * It provides very handy path processing, built-in character
 * encodings. It's cross-platform, but seems to work slower.
 *
 * 08.08.2018 results:
 *
 * 1) Linux Mint 19, valgrind --leak-check=yes
 *
 *	- std::filesystem:
 * 	heap usage: 171,666 allocs, 171,666 frees, 55,982,738 bytes alloc;
 *
 * 	- dirent.h:
 * 	heap usage: 10,813 allocs, 10,813 frees, 22,954 kbytes alloc
 *
 *	 Total items (files+folders) processed == 3441
 *
 *
 *	2) Windows 7, test app scanning eclipse folder, MSVC 17:
 *
 *	- std::filesystem:
 *	heap: (13,056 scanner itself) allocs, 55,982 kbytes alloc;
 *	files: 2849
 *	folders: 635
 *	time: 2676 non-cached, 1261 cached
 *
 *	-  dirent.h:
 *	heap: 18,962 (13,679 scanner itself) allocs,  1,124 kbytes alloc;
 *	files: 2849
 *	folders: 635
 *	time: 1542 non-cached, 1490 cached
 */

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <queue>
#include <vector>

namespace fs = std::filesystem;

#include "fs_scanner.hxx"

/* TODO On Windows failed skip directories with access denied. Even with
 * fs::directory_options::skip_permission_denied option scanner tries to
 * iterate over it and throws iterator error.
 */
/* TODO No exceptions, nor assert!
 */

/** FIXME
 * I don't like:
 *
 * 1) std::vector<directory*> and std::vector<file*>
 * as scanner private members. The only use is for
 * memory dealocation and get_all_files list.
 *
 * 2) ...
 */
[[maybe_unused]] constexpr unsigned int initial_file_list_size = 8;

namespace om
{

struct file;

struct directory
{
    directory*              parent = nullptr;
    std::vector<directory*> child_folders{};
    std::vector<file*>      child_files{};
    std::u8string           name{};
};

struct file
{
    size_t        size = 0;
    std::u8string name;
    std::u8string extension;
    directory*    parent = nullptr;

    [[nodiscard]] std::u8string get_full_name() const;
};

std::u8string file::get_full_name() const
{
    std::u8string result = name;
    if (!extension.empty())
    {
        result += '.';
        result += extension;
    }
    return result;
}

class scanner::impl
{
public:
    impl()                       = default;
    impl(const impl&)            = delete;
    impl(impl&&)                 = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(impl&&)      = delete;
    ~impl();

    void                 scan();
    static std::u8string get_directory_path(const directory*);
    static std::u8string get_file_path(const file*);
    directory*           find_directory_ptr(std::u8string_view);
    file*                find_file_ptr(std::u8string_view);

    directory                 root;
    std::vector<directory*>   folders;
    std::vector<file*>        files;
    size_t                    total_files{ 0 };
    size_t                    total_folders{ 0 };
    std::chrono::milliseconds scan_time{ 0 };
    bool                      is_initialized{ false };
    std::byte                 padding[7] = {};
};

scanner::impl::~impl()
{
    for (const auto& p : folders)
    {
        delete p;
    }
    for (const auto& p : files)
    {
        delete p;
    }
}

void scanner::impl::scan()
{
    const auto start = std::chrono::system_clock::now();
    std::queue<std::pair<fs::path, om::directory*>> recursion_queue;
    fs::path                                        path(root.name);
    recursion_queue.push(std::make_pair(path, &root));
    while (!recursion_queue.empty())
    {
        auto pair = recursion_queue.front();
        recursion_queue.pop();
        for (auto& p : fs::directory_iterator(pair.first))
        {
            if (fs::is_directory(p))
            {
                auto tmp = new om::directory;
                folders.push_back(tmp);
                tmp->name   = p.path().filename().u8string();
                tmp->parent = pair.second;
                pair.second->child_folders.push_back(tmp);
                recursion_queue.push(std::make_pair(p.path(), tmp));
                ++total_folders;
            }
            else if (fs::is_regular_file(p))
            {
                file* tmp = new om::file;
                files.push_back(tmp);
                tmp->extension = p.path().extension().u8string();
                tmp->name      = p.path().stem().u8string();
                if (!tmp->extension.empty())
                {
                    if (tmp->extension == u8".")
                        tmp->name += '.';
                    tmp->extension.erase(0, 1);
                }

                tmp->parent = pair.second;
                tmp->size   = fs::file_size(p);
                pair.second->child_files.push_back(tmp);
                ++total_files;
            }
        }
    }
    const auto finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    is_initialized = true;
    //  std::cout << "next sizes " << file_pool.get_next_size() << " "
    //            << directory_pool.get_next_size() << std::endl;
}

directory* scanner::impl::find_directory_ptr(std::u8string_view sv_path)
{
    directory* result = &root;
    fs::path   fs_path(sv_path);
    for (auto& p : fs_path)
    {
        auto it =
            std::find_if(begin(result->child_folders),
                         end(result->child_folders),
                         [&p](const directory* dir) { return dir->name == p; });
        if (it == result->child_folders.end())
        {
            return nullptr;
        }
        result = *it;
    }
    return result;
}

file* scanner::impl::find_file_ptr(std::u8string_view s_path)
{
    file*          result = nullptr;
    const fs::path fs_path(s_path);
    directory*     dir = find_directory_ptr(fs_path.parent_path().u8string());
    if (dir)
    {
        for (const auto& p : dir->child_files)
        {
            if (p->get_full_name() == fs_path.filename().u8string())
            {
                result = p;
            }
        }
    }
    return result;
}

std::u8string scanner::impl::get_directory_path(const directory* dir)
{
    fs::path result;
    for (directory* ptr0 = dir->parent; ptr0; ptr0 = ptr0->parent)
    {
        result = ptr0->name / result;
    }
    result /= dir->name;
    return result.u8string();
}

std::u8string scanner::impl::get_file_path(const file* fl)
{
    fs::path result;
    for (directory* ptr0 = fl->parent; ptr0; ptr0 = ptr0->parent)
    {
        result = ptr0->name / result;
    }
    result /= (fl->get_full_name());
    return result.u8string();
}

scanner::scanner(std::u8string_view path_)
    : pImpl(new scanner::impl)
{
    fs::path path(path_);

    if (path.is_relative())
    {
        path = (fs::current_path() / path).u8string();
    }
    if (!fs::exists(path))
        return;
    pImpl->root.name = path.u8string();
    pImpl->scan();
}

scanner::scanner(scanner&& scnr) noexcept
    : pImpl{ scnr.pImpl }
{
    scnr.pImpl = nullptr;
}

scanner& scanner::operator=(scanner&& scnr) noexcept
{
    delete pImpl;
    pImpl      = scnr.pImpl;
    scnr.pImpl = nullptr;
    return *this;
}

scanner::~scanner()
{
    delete pImpl;
    pImpl = nullptr;
}

size_t scanner::get_file_size(std::u8string_view name) const
{
    size_t result = std::numeric_limits<size_t>::max();
    file*  fl     = pImpl->find_file_ptr(name);
    if (fl)
        result = fl->size;
    return result;
}

bool scanner::is_file_exists(std::u8string_view path) const
{
    file* fl = pImpl->find_file_ptr(path);
    return fl ? true : false;
}

std::vector<file_info> scanner::get_files_with_extension(
    std::u8string_view path, std::u8string_view ext) const
{
    std::vector<file_info> result;
    // if (ext.front() == '.')  was supposed for user request like ".cxx"
    // with dot forward
    //    ext.erase(0, 1);
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (const auto& p : dir->child_files)
        {
            if (ext == p->extension)
            {
                file_info tmp;
                tmp.size     = p->size;
                tmp.abs_path = pImpl->get_file_path(p);
                result.push_back(tmp);
            }
        }
    }
    return result;
}

std::vector<file_info> scanner::get_files_with_name(
    std::u8string_view path, std::u8string_view name) const
{
    std::vector<file_info> result;
    if (name.empty())
    {
        return result;
    }
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (const auto& p : dir->child_files)
        {
            if (name == p->name)
            {
                file_info tmp;
                tmp.size     = p->size;
                tmp.abs_path = pImpl->get_file_path(p);
                result.push_back(tmp);
            }
        }
    }
    return result;
}

std::vector<file_info> scanner::get_files(std::u8string_view path) const
{
    std::vector<file_info> result;
    directory*             dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (const auto& p : dir->child_files)
        {
            file_info tmp;
            tmp.size     = p->size;
            tmp.abs_path = pImpl->get_file_path(p);
            result.push_back(tmp);
        }
    }
    return result;
}

std::vector<file_info> scanner::get_all_files() const
{
    std::vector<file_info> result;
    for (const auto& p : pImpl->files)
    {
        file_info tmp;
        tmp.size     = p->size;
        tmp.abs_path = pImpl->get_file_path(p);
        result.push_back(tmp);
    }
    return result;
}

scanner_report scanner::get_report() const
{
    scanner_report result;
    result.scan_time     = static_cast<size_t>(pImpl->scan_time.count());
    result.initialized   = pImpl->is_initialized;
    result.total_files   = pImpl->total_files;
    result.total_folders = pImpl->total_folders;
    return result;
}

} // namespace om
