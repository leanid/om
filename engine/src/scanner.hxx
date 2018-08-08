#pragma once

#ifdef _WIN32
#define SCNR_EXP __declspec(dllexport)
#else
#define SCNR_EXP
#endif

#include <string>
#include <string_view>

namespace om
{

struct file_info
{
    file_info() = default;
    file_info(const char* _path, size_t _size)
        : abs_path(_path)
        , size(_size){};
    std::string abs_path;
    size_t      size = 0;
};

class SCNR_EXP file_list final
{
public:
    file_list() = default;
    ~file_list();
    file_list(const file_list&);
    file_list& operator=(const file_list&);
    file_list(file_list&&);
    file_list& operator=(file_list&&);

    size_t     get_size() const { return size; }
    void       push(const file_info&);
    file_info& at(size_t) const;
    bool       empty() const;

    file_info* begin() const { return data; }
    file_info* end() const { return (data + size); }

private:
    file_info* data  = nullptr;
    size_t     size  = 0;
    size_t     space = 0;
};

struct scanner_report
{
    unsigned int scan_time     = 0;
    unsigned int total_files   = 0;
    unsigned int total_folders = 0;
    bool         initialized   = false;
};

class SCNR_EXP scanner final
{
public:
    scanner()               = delete;
    scanner(const scanner&) = delete;
    scanner& operator=(const scanner&) = delete;

    scanner(scanner&&);
    scanner& operator=(scanner&&);

    explicit scanner(std::string_view path);

    size_t get_file_size(std::string_view name) const;

    // Function return file size in bytes, if file exists. Otherwise -1.
    // Null is a valid return value. The "name" parameter must be
    // a relative path, where scanner was launched from, with
    // and extension if presents. Invalid requests like empty string
    // or incorrect (non-exist) path will also return -1;

    bool is_file_exists(std::string_view name) const;

    // Function returns true if file exists on a given path. Invalid
    // requests like empty or incorrect path or name will return false;

    file_list get_files_with_extension(std::string_view path,
                                       std::string_view extn) const;

    // Function return a file_list container, which holds file_info
    // structures for given requirements. List will be empty if
    // nothing was found. Empty path produces search in scanner's
    // root directory.  Empty extension makes search for files
    // w/o extension. Incorrect parameters (i.e. invalid path
    // or incorrect extension) will return an empty container.

    file_list get_files_with_name(std::string_view path,
                                  std::string_view name) const;

    // Function return a file_list container, which holds file_info
    // structures for given requirements. List will be empty if
    // nothing was found. Empty path produces search in scanner's
    // root directory.  Empty name is an incorrect value.
    // Incorrect parameters will return an empty container.

    file_list get_files(std::string_view path) const;

    // Function return a file_list container, which holds file_info
    // structure for all files in a  given path. List will be empty
    // if given path's directory doesn't contain any files.
    // Empty path produces search in scanner's root directory.
    // Non-exist path will return an empty container.

    file_list get_all_files() const;

    // Function return a file_list container, which holds file_info
    // structure for all files that scanner found in all directories.

    scanner_report get_report() const;

    // Function return a scanner_report structure, which contains
    // information about scanner internal values and states.

    ~scanner();

private:
    class impl;
    impl* pImpl;
};

} // namespace om
