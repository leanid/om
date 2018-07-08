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
/* TODO On Windows failed skip directories with access denied. Even with
 * fs::directory_options::skip_permission_denied option scanner tries to
 * iterate over it and throws iterator error.
 */
/* TODO Add scanner's destructor that will free all pointers (directories,
 * files, etc.)
 */
/* TODO No exceprions, nor assert!
 */

namespace fs = std::experimental::filesystem;

namespace om
{

constexpr unsigned int initial_file_list_size = 8;

om::file_list::file_list()
    : data(nullptr)
    , sz(0)
    , space(0)
{
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
    std::vector<om::directory*> dirs;
    std::vector<om::file*>      files;
    om::directory               root;

    void        scan();
    bool        single_folder_scan(directory*);
    directory*  find_directory_ptr(const std::string&);
    file*       find_file_ptr(const std::string&);
    std::string get_file_path(const file*);

    bool                      is_initialized{ false };
    bool                      root_scanned{ false };
    size_t                    total_files_count{ 0 };
    size_t                    total_files_size{ 0 };
    std::chrono::milliseconds scan_time{ 0 };
};

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
        assert(fs::exists(path)); // FIXME no assert?
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

scanner_report scanner::get_report() const
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
            tmp->name      = p.path().stem();
            tmp->parent    = dir;
            tmp->extension = p.path().extension();
            tmp->size      = fs::file_size(p);
            dir->child_files.push_back(tmp);
            files.push_back(tmp);
        }
    }
    return true;
}

void scanner::impl::scan()
{
    assert(is_initialized); // FIXME no assert?
    std::chrono::time_point<std::chrono::system_clock> start, finish;
    start = std::chrono::system_clock::now();
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
    // May be won't need it
    /*   std::sort(dirs.begin(), dirs.end(), [](om::directory* a, om::directory*
       b) { return a->name < b->name;
       });
       std::sort(files.begin(), files.end(),
                 [](file* a, file* b) { return a->name < b->name; });*/
    root_scanned = true;
    return;
}

file* scanner::impl::find_file_ptr(const std::string& _path)
{
    file*      result = nullptr;
    fs::path   path(_path);
    directory* dir = find_directory_ptr(path.parent_path());
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

int scanner::get_file_size(const std::string& name) const
{
    int   result = -1;
    file* fl     = pImpl->find_file_ptr(name);
    if (fl)
        result = fl->size;
    return result;
}

bool scanner::is_file_exists(const std::string& name) const
{
    file* fl = pImpl->find_file_ptr(name);
    return fl ? true : false;
}

std::string scanner::impl::get_file_path(const file* fl)
{
    fs::path result;
    for (directory* ptr = fl->parent; ptr; ptr = ptr->parent)
    {
        result = ptr->name / result;
    }
    result /= (fl->get_full_name());
    return result;
}

file_list scanner::get_all_files_with_extension(std::string        extn,
                                                const std::string& path) const
{
    if (extn.front() != '.')
        extn.insert(extn.begin(), '.');
    file_list  result;
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

} // namespace om
