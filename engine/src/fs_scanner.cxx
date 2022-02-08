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

#define USE_DIRENT
//#define USE_STD_FILESYSTEM

#ifdef USE_DIRENT
#include <dirent.h>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#if defined(__unix__)
#include <unistd.h> //for getcwd()
#elif defined(WIN32)
#include <direct.h> //for getcwd()
#endif
#endif

#ifdef USE_STD_FILESYSTEM
#include <filesystem>
#include <queue>
namespace fs = std::filesystem;
#endif

#include "fs_scanner.hxx"
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <string_view>
#include <vector>

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
    directory(){};
    directory*              parent = nullptr;
    std::vector<directory*> child_folders{};
    std::vector<file*>      child_files{};
    std::string             name{};
};

struct file
{
    size_t      size = 0;
    std::string name;
    std::string extension;
    directory*  parent = nullptr;

    std::string get_full_name() const;
};

std::string file::get_full_name() const
{
    std::string result = name;
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
    impl(){};
    ~impl();

    void        scan();
    std::string get_directory_path(const directory*);
    std::string get_file_path(const file*);
    directory*  find_directory_ptr(std::string_view);
    file*       find_file_ptr(std::string_view);

    om::directory             root;
    std::vector<directory*>   folders;
    std::vector<file*>        files;
    bool                      is_initialized{ false };
    size_t                    total_files{ 0 };
    size_t                    total_folders{ 0 };
    std::chrono::milliseconds scan_time{ 0 };
};

scanner::impl::~impl()
{
    for (auto& p : folders)
    {
        delete p;
    }
    for (auto& p : files)
    {
        delete p;
    }
}

#ifdef USE_STD_FILESYSTEM
void scanner::impl::scan()
{
    auto start = std::chrono::system_clock::now();
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
                om::directory* tmp = new om::directory;
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
                    if (tmp->extension == ".")
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
    auto finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    is_initialized = true;
    //  std::cout << "next sizes " << file_pool.get_next_size() << " "
    //            << directory_pool.get_next_size() << std::endl;
    return;
}

directory* scanner::impl::find_directory_ptr(std::string_view sv_path)
{
    directory* result = &root;
    fs::path   fs_path(sv_path);
    for (auto& p : fs_path)
    {
        auto it =
            std::find_if(result->child_folders.begin(),
                         result->child_folders.end(),
                         [&p](const directory* dir) { return dir->name == p; });
        if (it == result->child_folders.end())
        {
            return nullptr;
        }
        result = *it;
    }
    return result;
}

file* scanner::impl::find_file_ptr(std::string_view s_path)
{
    file*      result = nullptr;
    fs::path   fs_path(s_path);
    directory* dir = find_directory_ptr(fs_path.parent_path().string());
    if (dir)
    {
        for (auto& p : dir->child_files)
        {
            if (p->get_full_name() == fs_path.filename().string())
            {
                result = p;
            }
        }
    }
    return result;
}

std::string scanner::impl::get_directory_path(const directory* dir)
{
    fs::path result;
    for (directory* ptr0 = dir->parent; ptr0; ptr0 = ptr0->parent)
    {
        result = ptr0->name / result;
    }
    result /= dir->name;
    return result.string();
}

std::string scanner::impl::get_file_path(const file* fl)
{
    fs::path result;
    for (directory* ptr0 = fl->parent; ptr0; ptr0 = ptr0->parent)
    {
        result = ptr0->name / result;
    }
    result /= (fl->get_full_name());
    return result.string();
}

scanner::scanner(std::string_view path_)
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
#endif

#ifdef USE_DIRENT
void scanner::impl::scan()
{

    std::vector<om::directory*> recursion_stack;
    recursion_stack.reserve(64);
    recursion_stack.push_back(&root);
    errno    = 0;
    DIR* dir = nullptr;

    while (!recursion_stack.empty())
    {
        om::directory* tmp = recursion_stack.back();
        recursion_stack.pop_back();

        dir = opendir(get_directory_path(tmp).c_str());

        if (dir)
        {
            struct dirent* entry = nullptr;
            while ((entry = readdir(dir)) != nullptr)
            {
                if (DT_DIR == entry->d_type)
                {
                    if (strcmp(entry->d_name, ".") == 0 ||
                        strcmp(entry->d_name, "..") == 0)
                        continue;
                    om::directory* dir = new om::directory;
                    folders.push_back(dir);
                    dir->name   = entry->d_name;
                    dir->parent = tmp;
                    tmp->child_folders.push_back(dir);
                    recursion_stack.push_back(dir);
                    ++total_folders;
                }
                else if (DT_REG == entry->d_type)
                {
                    file* fl = new om::file;
                    files.push_back(fl);
                    fl->name      = entry->d_name;
                    int split_pos = fl->name.find_last_of('.');
                    if (split_pos > 0 &&
                        (split_pos != int(fl->name.length() - 1)))
                    {
                        fl->extension =
                            fl->name.substr(split_pos + 1, fl->name.length());
                        fl->name.resize(split_pos);
                    }
                    fl->parent = tmp;
                    struct stat statbuf;
                    if (0 == stat(get_file_path(fl).c_str(), &statbuf))
                        fl->size = statbuf.st_size;
                    tmp->child_files.push_back(fl);
                    ++total_files;
                }
            }
            closedir(dir);
        }
        else if (errno == EACCES)
        {
            continue;
            // if access denied we just skip directory
        }
        else
        {
            return;
            // unspecified error
        }
    }
    is_initialized = true;
    return;
}

directory* scanner::impl::find_directory_ptr(std::string_view sv_path)
{

    directory* result = &root;
    if (sv_path.empty())
        return result;
    int begin_index = 0;
    int seek_pos    = 0;
    int count       = 0;

    while (seek_pos >= 0)
    {
        seek_pos = sv_path.find_first_of('/', begin_index);

        if (seek_pos < 0)
            count = sv_path.length() - begin_index;
        else
            count = seek_pos - begin_index;

        std::string_view tmp(sv_path.data() + begin_index, count);
        auto             it = std::find_if(result->child_folders.begin(),
                               result->child_folders.end(),
                               [&tmp](const directory* dir)
                               { return dir->name == tmp; });
        if (it == result->child_folders.end())
        {
            return nullptr;
        }
        result      = *it;
        begin_index = seek_pos + 1;
    }
    return result;
}

file* scanner::impl::find_file_ptr(std::string_view sv_path)
{
    file* result = nullptr;

    int filename_starts_from = 0;
    int pathname_ends_at     = 0;
    int seek_pos             = sv_path.find_last_of('/');
    if (seek_pos > 0)
    // at least 3 characters needed to specify
    // relative file path ("c/a"), so seek_pos
    // must be at least 1 (not 0);
    {
        filename_starts_from = seek_pos + 1;
        pathname_ends_at     = seek_pos;
    }

    std::string_view filename(sv_path.data() + filename_starts_from,
                              sv_path.length() - filename_starts_from);
    std::string_view path(sv_path.data(), pathname_ends_at);
    directory*       dir = find_directory_ptr(path);
    if (dir)
    {
        for (auto& p : dir->child_files)
        {
            if (p->get_full_name() == filename)
            {
                result = p;
            }
        }
    }
    return result;
}

std::string scanner::impl::get_directory_path(const directory* dir)
{

    char ch_path[PATH_MAX];
    int  write_pos     = PATH_MAX - 1;
    ch_path[write_pos] = '\0';
    --write_pos;
    while (dir)
    {
        for (auto p = dir->name.rbegin(); p != dir->name.rend(); ++p)
        {
            ch_path[write_pos] = *p;
            --write_pos;
        }
        ch_path[write_pos] = '/';
        --write_pos;
        dir = dir->parent;
    }
    std::string result(ch_path + write_pos + 1 + 1);
    // plus one because of last decrement and
    // one more plus for last '/' symbol
    return result;
}

std::string scanner::impl::get_file_path(const file* fl)
{
    char ch_path[PATH_MAX];
    int  write_pos     = PATH_MAX - 1;
    ch_path[write_pos] = '\0';
    --write_pos;
    if (!fl->extension.empty())
    {
        for (auto p = fl->extension.rbegin(); p != fl->extension.rend(); ++p)
        {
            ch_path[write_pos] = *p;
            --write_pos;
        }
        ch_path[write_pos] = '.';
        --write_pos;
    }
    for (auto p = fl->name.rbegin(); p != fl->name.rend(); ++p)
    {
        ch_path[write_pos] = *p;
        --write_pos;
    }
    ch_path[write_pos] = '/';
    --write_pos;
    om::directory* dir = fl->parent;
    while (dir)
    {
        for (auto p = dir->name.rbegin(); p != dir->name.rend(); ++p)
        {
            ch_path[write_pos] = *p;
            --write_pos;
        }
        ch_path[write_pos] = '/';
        --write_pos;
        dir = dir->parent;
    }
    std::string result(ch_path + (write_pos + 1 + 1));
    return result;
}

scanner::scanner(std::string_view sv_path)
    : pImpl(new scanner::impl)
{
    bool absolute = false;
#if defined(__unix__)
    if (sv_path.size() > 1)
    {
        if (sv_path.front() == '/')
        {
            absolute = true;
        }
    }
#elif defined(WIN32)
    if (sv_path.size() > 2)
    {
        if ((sv_path.at(2) == '/') && (sv_path.at(1) == ':') &&
            isupper(sv_path.at(0)) && isalpha(sv_path.at(0)))
        {
            absolute = true;
        }
    }
#endif
    if (!absolute)
    {
        char        cwd[PATH_MAX];
        const char* path = getcwd(cwd, sizeof(cwd));
        if (!path)
            return;
        pImpl->root.name = path;
        if (!sv_path.empty())
        {
            pImpl->root.name += "/";
            pImpl->root.name += sv_path;
        }
    }
    else
    {
        pImpl->root.name = sv_path;
    }
    auto start = std::chrono::system_clock::now();
    pImpl->scan();
    auto finish = std::chrono::system_clock::now();
    pImpl->scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
}

#endif

scanner::scanner(scanner&& scnr)
    : pImpl{ scnr.pImpl }
{
    scnr.pImpl = nullptr;
}

scanner& scanner::operator=(scanner&& scnr)
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

size_t scanner::get_file_size(std::string_view name) const
{
    size_t result = -1;
    file*  fl     = pImpl->find_file_ptr(name);
    if (fl)
        result = fl->size;
    return result;
}

bool scanner::is_file_exists(std::string_view path) const
{
    file* fl = pImpl->find_file_ptr(path);
    return fl ? true : false;
}

std::vector<file_info> scanner::get_files_with_extension(
    std::string_view path, std::string_view extn) const
{
    std::vector<file_info> result;
    // if (extn.front() == '.')  was supposed for user request like ".cxx"
    // with dot forward
    //    extn.erase(0, 1);
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (auto& p : dir->child_files)
        {
            if (extn == p->extension)
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

std::vector<file_info> scanner::get_files_with_name(std::string_view path,
                                                    std::string_view name) const
{
    std::vector<file_info> result;
    if (name.empty())
        return result;
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (auto& p : dir->child_files)
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

std::vector<file_info> scanner::get_files(std::string_view path) const
{
    std::vector<file_info> result;
    directory*             dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (auto& p : dir->child_files)
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
    for (auto& p : pImpl->files)
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
    result.scan_time     = pImpl->scan_time.count();
    result.initialized   = pImpl->is_initialized;
    result.total_files   = pImpl->total_files;
    result.total_folders = pImpl->total_folders;
    return result;
}

} // namespace om
