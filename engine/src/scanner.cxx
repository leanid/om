#include "scanner.hxx"
#include <algorithm>
#include <assert.h>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <experimental/filesystem>
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
namespace fs                                  = std::experimental::filesystem;

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
    result += extension;
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

    om::directory                     root;
    boost::object_pool<om::directory> directory_pool;
    boost::object_pool<om::file>      file_pool;

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

scanner::impl::~impl() {}

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
                om::directory* const tmp = directory_pool.construct();
                // om::directory* tmp = new om::directory;
                tmp->name   = p.path().filename().string();
                tmp->parent = pair.second;
                pair.second->child_folders.push_back(tmp);
                recursion_queue.push(std::make_pair(p, tmp));
                ++total_folders;
            }
            else if (fs::is_regular_file(p))
            {
                file* const tmp = file_pool.construct();
                // file* tmp      = new om::file;
                tmp->name      = p.path().stem().string();
                tmp->parent    = pair.second;
                tmp->extension = p.path().extension().string();
                tmp->size      = fs::file_size(p);
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
    if (extn.empty())
        return result;
    if (extn.front() != '.')
        extn.insert(extn.begin(), '.');
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
