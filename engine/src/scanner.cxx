//#define USE_DIRENT
#define USE_STD_FILESYSTEM
// XXX #define USE_BOOST

#ifdef USE_DIRENT
#include <dirent.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef USE_STD_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#endif

#ifdef USE_BOOST
#include <boost/filesystem.hpp>
#include <boost/pool/object_pool.hpp>
#endif

#include "scanner.hxx"
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>
/* TODO Locale dependency. We have to use std::wstring, std::wcout
 * to handle paths with russian names. Otherwise scanner won't find
 * any entries;
 */
/* TODO On Windows failed skip directories with access denied. Even with
 * fs::directory_options::skip_permission_denied option scanner tries to
 * iterate over it and throws iterator error.
 */
/* TODO Add scanner's impl destructor that will free all pointers (directories,
 * files, etc.)
 */
/* TODO No exceprions, nor assert!
 */

constexpr unsigned int initial_file_list_size = 8;

namespace om
{

om::file_list::file_list()
    : data(nullptr)
    , sz(0)
    , space(0)
{
}

file_list::file_list(const file_list& lst)
    : data{ new file_info[lst.space] }
    , sz{ lst.sz }
    , space{ lst.space }
{
    std::copy(lst.data, lst.data + lst.sz, data);
}

file_list& file_list::operator=(const file_list& lst)
{
    if (this == &lst)
    {
        return *this;
    }
    if (lst.sz <= space)
    {
        std::copy(lst.data, lst.data + lst.sz, data);
        sz = lst.sz;
        return *this;
    }
    file_info* ptr = new file_info[lst.space];
    std::copy(lst.data, lst.data + lst.sz, ptr);
    delete[] data;
    data  = ptr;
    space = lst.space;
    sz    = lst.sz;
    return *this;
}

file_list::file_list(file_list&& lst)
    : data{ lst.data }
    , sz{ lst.sz }
    , space{ lst.space }
{
    lst.data  = nullptr;
    lst.sz    = 0;
    lst.space = 0;
}

file_list& file_list::operator=(file_list&& lst)
{
    delete[] data;
    data      = lst.data;
    sz        = lst.sz;
    space     = lst.space;
    lst.data  = nullptr;
    lst.sz    = 0;
    lst.space = 0;
    return *this;
}

void om::file_list::push(const file_info& inf)
{
    if (space == sz)
    {
        unsigned int newalloc = space * 2;
        if (!space)
            newalloc = initial_file_list_size;

        file_info* tmp = new file_info[newalloc];
        if (sz)
        {
            for (unsigned int i = 0; i < sz; ++i)
            {
                tmp[i] = data[i];
            }
        }
        delete[] data;
        data  = tmp;
        space = newalloc;
    }
    data[sz] = inf;
    ++sz;
}

om::file_info& om::file_list::at(unsigned int pos) const
{
    if (pos >= sz)
        throw; // FIXME no throw?
    return data[pos];
}

bool om::file_list::empty() const
{
    return sz ? false : true;
}

om::file_list::~file_list()
{
    delete[] data;
}

class file;

class directory
{
public:
    directory(){};
    directory*              parent = nullptr;
    std::vector<directory*> child_folders{};
    std::vector<file*>      child_files{};
    std::string             name{};
};

class file
{
public:
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
    impl();
    ~impl();

    void        scan();
    directory*  find_directory_ptr(const std::string&);
    file*       find_file_ptr(const std::string&);
    std::string get_directory_path(const directory*);
    std::string get_file_path(const file*);

#if defined USE_DIRENT || defined USE_STD_FILESYSTEM
    std::vector<directory*> folders;
    std::vector<file*>      files;
#endif
#ifdef USE_BOOST
    boost::object_pool<directory> folders;
    boost::object_pool<file>      files;
#endif
    om::directory             root;
    bool                      is_initialized;
    size_t                    total_files;
    size_t                    total_folders;
    std::chrono::milliseconds scan_time;
};

scanner::impl::impl()
    : root{}
    , is_initialized{ false }
    , total_files{ 0 }
    , total_folders{ 0 }
    , scan_time{ 0 }
{
    // directory_pool.set_next_size(512);
    // file_pool.set_next_size(512);
}

scanner::impl::~impl()
{
#if defined USE_DIRENT || defined USE_STD_FILESYSTEM
    for (auto p : folders)
    {
        delete p;
    }
    for (auto p : files)
    {
        delete p;
    }
#endif
}

#ifdef USE_STD_FILESYSTEM
void scanner::impl::scan()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, finish;
    start = std::chrono::system_clock::now();
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
                tmp->name   = p.path().filename().string();
                tmp->parent = pair.second;
                pair.second->child_folders.push_back(tmp);
                recursion_queue.push(std::make_pair(p, tmp));
                ++total_folders;
            }
            else if (fs::is_regular_file(p))
            {
                file* tmp = new om::file;
                files.push_back(tmp);
                tmp->extension = p.path().extension().string();
                tmp->name      = p.path().stem().string();
                if (!tmp->extension.empty())
                {
                    if (tmp->extension == ".")
                        tmp->name += '.';
                    tmp->extension.erase(0, 1);
                }

                tmp->parent = pair.second;

                tmp->size = fs::file_size(p);
                pair.second->child_files.push_back(tmp);
                ++total_files;
            }
        }
    }
    finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    is_initialized = true;
    //  std::cout << "next sizes " << file_pool.get_next_size() << " "
    //            << directory_pool.get_next_size() << std::endl;
    return;
}

directory* scanner::impl::find_directory_ptr(const std::string& _path)
{
    directory* result = &root;
    fs::path   path(_path);

    for (auto p : path)
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

file* scanner::impl::find_file_ptr(const std::string& _path)
{
    file*      result = nullptr;
    fs::path   path(_path);
    directory* dir = find_directory_ptr(path.parent_path().string());
    if (dir)
    {
        for (auto p : dir->child_files)
        {
            if (p->get_full_name() == path.filename())
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

scanner::scanner(const std::string& path_)
    : pImpl(new scanner::impl)
{
    fs::path path(path_);

    if (path.is_relative())
    {
        path = (fs::current_path() / path).string();
    }
    if (!fs::exists(path))
        return;
    pImpl->root.name = path.string();
    pImpl->scan();
}
#endif
#ifdef USE_DIRENT

void scanner::impl::scan()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, finish;
    start = std::chrono::system_clock::now();

    std::queue<om::directory*> recursion_queue;
    recursion_queue.push(&root);
    while (!recursion_queue.empty())
    {
        om::directory* tmp = recursion_queue.front();
        recursion_queue.pop();
        DIR*           dir;
        struct dirent* entry;

        if (!(dir = opendir(get_directory_path(tmp).c_str())))
        {
            return;
        }
        while (nullptr != (entry = readdir(dir)))
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
                recursion_queue.push(dir);
                ++total_folders;
            }
            else if (DT_REG == entry->d_type)
            {
                file* fl = new om::file;
                files.push_back(fl);
                fl->name      = entry->d_name;
                int split_pos = fl->name.find_last_of('.');
                if (split_pos > 0 && (split_pos != int(fl->name.length() - 1)))
                {
                    fl->extension =
                        fl->name.substr(split_pos + 1, fl->name.length());
                    fl->name.resize(split_pos);
                }
                fl->parent = tmp;
                struct stat statbuf;
                std::string t = get_file_path(fl);
                if (0 == stat(t.c_str(), &statbuf))
                    fl->size = statbuf.st_size;
                tmp->child_files.push_back(fl);
                ++total_files;
            }
        }
        closedir(dir);
    }

    finish = std::chrono::system_clock::now();
    scan_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    is_initialized = true;
    //  std::cout << "next sizes " << file_pool.get_next_size() << " "
    //            << directory_pool.get_next_size() << std::endl;
    return;
}

directory* scanner::impl::find_directory_ptr(const std::string& _path)
{

    directory* result = &root;
    if (_path.empty())
        return result;
    std::stringstream ss(_path);
    std::string       tmp_str;

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

file* scanner::impl::find_file_ptr(const std::string& _path)
{
    file*       result   = nullptr;
    std::string filename = _path;
    std::string path     = "";

    int split_pos = _path.find_last_of('/');
    if (split_pos >= 0)
    {
        filename = _path.substr(split_pos + 1, _path.length());
        path     = _path.substr(0, split_pos);
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
    std::vector<std::string> dir_list;
    while (dir)
    {
        dir_list.push_back(dir->name);
        dir = dir->parent;
    }
    std::string result;
    for (int i = dir_list.size() - 1; i > 0; --i)
    {
        result += dir_list.at(i);
        result += '/';
    }
    result += dir_list.at(0);
    return result;
}

std::string scanner::impl::get_file_path(const file* fl)
{
    std::string result;
    for (directory* ptr = fl->parent; ptr; ptr = ptr->parent)
    {
        result.insert(0, ptr->name);
        if (result.front() != '/')
            result.insert(0, "/");
    }
    return (result + "/" + fl->get_full_name());
}

scanner::scanner(const std::string& path_)
    : pImpl(new scanner::impl)
{
    if (path_.front() != '/')
    {
        char        cwd[PATH_MAX];
        const char* path = getcwd(cwd, sizeof(cwd));
        if (!path)
            return;
        std::string current_dir(path);
        pImpl->root.name = current_dir + "/" + path_;
    }
    else
    {
        pImpl->root.name = path_;
    }

    pImpl->scan();
}

#endif

scanner::~scanner()
{
    delete pImpl;
    pImpl = nullptr;
}

int scanner::get_file_size(const std::string& name) const
{
    int result = -1;

    file* fl = pImpl->find_file_ptr(name);
    if (fl)
        result = fl->size;
    return result;
}

bool scanner::is_file_exists(const std::string& name) const
{
    file* fl = pImpl->find_file_ptr(name);
    return fl ? true : false;
}

file_list scanner::get_all_files_with_extension(std::string        extn,
                                                const std::string& path) const
{
    file_list result;
    if (extn.front() == '.')
        extn.erase(0, 1);
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

file_list scanner::get_all_files_with_name(const std::string& name,
                                           const std::string& path) const
{
    file_list  result;
    directory* dir = pImpl->find_directory_ptr(path);
    if (dir)
    {
        if (name.empty())
        {
            for (auto p : dir->child_files)
            {
                file_info tmp;
                tmp.size     = p->size;
                tmp.abs_path = pImpl->get_file_path(p);
                result.push(tmp);
            }
        }
        else
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
