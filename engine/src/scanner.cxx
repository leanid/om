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
 * 	heap usage: 10,813 allocs, 10,813 frees, 22,954,887 bytes alloc
 *
 *	 Total items (files+folders) processed == 3441
 *
 */

//#define USE_DIRENT
#define USE_STD_FILESYSTEM

#ifdef USE_DIRENT
//#include <direct.h> //for win32
#include <dirent.h>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef USE_STD_FILESYSTEM
#include <filesystem>
#include <queue>
namespace fs = std::filesystem;
#endif

#include "scanner.hxx"
#include <assert.h>
#include <chrono>
//#include <codecvt> //for win32
#include <functional>
#include <string_view>
#include <utility>
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
 * 1) file_list container. It's pretty useless, as
 * it acts exactly like std::vector. So there is no
 * real reason to have it here because we use
 * std::vector in many places.
 *
 * 2) std::vector<directory*> and std::vector<file*>
 * as scanner private members. The only use is for
 * memory dealocation and get_all_files list.
 *
 * 3) use of std::stringstream. As there is the only
 * one place where it used.
 *
 * 4)
 */
constexpr unsigned int initial_file_list_size = 8;

namespace om
{

file_list::file_list(const file_list& lst)
    : data{ new file_info[lst.space] }
    , size{ lst.size }
    , space{ lst.space }
{
    std::copy(lst.data, lst.data + lst.size, data);
}

file_list& file_list::operator=(const file_list& lst)
{
    if (this == &lst)
    {
        return *this;
    }
    if (lst.size <= space)
    {
        std::copy(lst.data, lst.data + lst.size, data);
        size = lst.size;
        return *this;
    }
    file_info* ptr = new file_info[lst.space];
    std::copy(lst.data, lst.data + lst.size, ptr);
    delete[] data;
    data  = ptr;
    space = lst.space;
    size  = lst.size;
    return *this;
}

file_list::file_list(file_list&& lst)
    : data{ lst.data }
    , size{ lst.size }
    , space{ lst.space }
{
    lst.data  = nullptr;
    lst.size  = 0;
    lst.space = 0;
}

file_list& file_list::operator=(file_list&& lst)
{
    delete[] data;
    data      = lst.data;
    size      = lst.size;
    space     = lst.space;
    lst.data  = nullptr;
    lst.size  = 0;
    lst.space = 0;
    return *this;
}

void om::file_list::push(const file_info& inf)
{
    if (space == size)
    {
        unsigned int newalloc = space * 2;
        if (!space)
            newalloc = initial_file_list_size;

        file_info* tmp = new file_info[newalloc];
        if (size)
        {
            for (unsigned int i = 0; i < size; ++i)
            {
                tmp[i] = data[i];
            }
        }
        delete[] data;
        data  = tmp;
        space = newalloc;
    }
    data[size] = inf;
    ++size;
}

om::file_info& om::file_list::at(size_t pos) const
{
    if (pos >= size)
        throw; // FIXME no throw?
    return data[pos];
}

bool om::file_list::empty() const
{
    return size ? false : true;
}

om::file_list::~file_list()
{
    delete[] data;
}

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
    for (auto p : folders)
    {
        delete p;
    }
    for (auto p : files)
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
        for (auto p : fs::directory_iterator(pair.first))
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

directory* scanner::impl::find_directory_ptr(std::string_view s_path)
{
    directory* result = &root;
    fs::path   fs_path(s_path);
    for (auto p : fs_path)
    {
        auto it = std::find_if(
            result->child_folders.begin(), result->child_folders.end(),
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
        for (auto p : dir->child_files)
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
    for (directory* ptr = dir->parent; ptr; ptr = ptr->parent)
    {
        result = ptr->name / result;
    }
    result /= dir->name;
    return result.string();
}

std::string scanner::impl::get_file_path(const file* fl)
{
    fs::path result;
    for (directory* ptr = fl->parent; ptr; ptr = ptr->parent)
    {
        result = ptr->name / result;
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
#ifdef _WIN32
                    fl->size = entry->d_file_size;
#endif
#ifdef __unix__

                    struct stat statbuf;
                    if (0 == stat(get_file_path(fl).c_str(), &statbuf))
                        fl->size = statbuf.st_size;
#endif
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
    std::string       tmp_str;
    std::stringstream ss(sv_path.data());

    while (getline(ss, tmp_str, '/'))
    {
        auto it = std::find_if(
            result->child_folders.begin(), result->child_folders.end(),
            [&tmp_str](const directory* dir) { return dir->name == tmp_str; });
        if (it == result->child_folders.end())
        {
            return nullptr;
        }
        result = *it;
    }
    return result;
}

file* scanner::impl::find_file_ptr(std::string_view sv_path)
{
    file*       result = nullptr;
    std::string filename(sv_path);
    std::string path;

    int split_pos = sv_path.find_last_of('/');
    if (split_pos >= 0)
    {
        filename = sv_path.substr(split_pos + 1, sv_path.length());
        path     = sv_path.substr(0, split_pos);
    }

    directory* dir = find_directory_ptr(path);
    if (dir)
    {
        for (auto p : dir->child_files)
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
    if (sv_path.front() != '/')
    {
        char        cwd[PATH_MAX];
        const char* path = getcwd(cwd, sizeof(cwd));
        if (!path)
            return;
        pImpl->root.name = path;
        pImpl->root.name += "/";
        pImpl->root.name += sv_path;
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
    int   result = -1;
    file* fl     = pImpl->find_file_ptr(name);
    if (fl)
        result = fl->size;
    return result;
}

bool scanner::is_file_exists(std::string_view path) const
{
    file* fl = pImpl->find_file_ptr(path);
    return fl ? true : false;
}

file_list scanner::get_files_with_extension(std::string_view path,
                                            std::string_view extn) const
{
    file_list result;
    // if (extn.front() == '.')  was supposed for user request like ".cxx"
    // with dot forward
    //    extn.erase(0, 1);
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (auto p : dir->child_files)
        {
            if (extn == p->extension)
            {
                file_info tmp;
                tmp.size     = p->size;
                tmp.abs_path = pImpl->get_file_path(p);
                result.push(tmp);
            }
        }
    }
    return result;
}

file_list scanner::get_files_with_name(std::string_view path,
                                       std::string_view name) const
{
    file_list result;
    if (name.empty())
        return result;
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (auto p : dir->child_files)
        {
            if (name == p->name)
            {
                file_info tmp;
                tmp.size     = p->size;
                tmp.abs_path = pImpl->get_file_path(p);
                result.push(tmp);
            }
        }
    }
    return result;
}

file_list scanner::get_files(std::string_view path) const
{
    file_list  result;
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        for (auto p : dir->child_files)
        {
            file_info tmp;
            tmp.size     = p->size;
            tmp.abs_path = pImpl->get_file_path(p);
            result.push(tmp);
        }
    }
    return result;
}

file_list scanner::get_all_files() const
{
    file_list result;
    for (auto p : pImpl->files)
    {
        file_info tmp;
        tmp.size     = p->size;
        tmp.abs_path = pImpl->get_file_path(p);
        result.push(tmp);
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
