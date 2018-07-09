#pragma once

#include <string>

namespace om
{

struct file_info
{
    file_info()
        : abs_path("")
        , size(0){};
    file_info(const char* _path, unsigned int _size)
        : abs_path(_path)
        , size(_size){};
    std::string  abs_path;
    unsigned int size;
};

class file_list final
{
public:
    file_list();
    ~file_list();
    file_list(const file_list&);
    file_list& operator=(const file_list&);
    file_list(file_list&&);
    file_list& operator=(file_list&&);

    unsigned int size() const { return sz; }
    void         push(const file_info&);
    file_info&   at(unsigned int) const;
    bool         empty() const;

    file_info* begin() const { return data; }
    file_info* end() const { return (data + sz); }

private:
    file_info*   data;
    unsigned int sz;
    unsigned int space;
};

struct scanner_report
{
    unsigned int scan_time      = 0;
    unsigned int total_files    = 0;
    unsigned int total_folders  = 0;
    bool         is_initialized = false;
    bool         scan_perfomed  = false;
};

class scanner final
{
public:
    // TODO Implement all the constructors
    scanner()               = delete;
    scanner(const scanner&) = delete;
    scanner& operator=(const scanner&) = delete;
    scanner(scanner&&)                 = delete;
    scanner& operator=(scanner&&) = delete;

    explicit scanner(const std::string& path);

    int get_file_size(
        const std::string& name) const; // may be replace with size_t?
    /**
     * Function return file size in bytes, if file exists. Otherwise -1.
     * Null is a valid return value. An input argument is a relative path
     * to the directory, where scanner was launched from. Invalid requests
     * such like path with empty string or without file extension (if present
     * in actual file) will also return -1;
     */

    bool is_file_exists(const std::string& name) const;

    /**
     * Function return true if file is exists on a given path. Invalid
     * requests such like path with empty string or without file extension
     * (if present in actual file) will return false;
     */

    file_list get_all_files_with_extension(std::string        extn,
                                           const std::string& path) const;
    /**
     * Function return a file_list container, which holds file_info structures
     * for given requirements. Empty string is valid as input parameter
     * and produce search in scanner's root directory. If one would specify
     * incorrect path (i.e. not valid or absent),function will return an empty
     * container. Empty string as file's extension argument will return a
     * non-empty container if path will contain files w/o extension.
     */

    file_list get_all_files_with_name(const std::string& name,
                                      const std::string& path) const;
    /**
     * Function return a file_list container, which holds file_info structures
     * for given requirements. Empty string is valid as input parameter
     * and produce search in scanner's root directory. If one would specify
     * incorrect path (i.e. not valid or absent),function will return an empty
     * container. Empty string for file's extension argument will produce search
     * for all files on given path.
     */

    scanner_report get_report() const;
    /**
     * Function return a scanner_report structure, which contains information
     * about scanner internal values and states.
     */

    ~scanner();

private:
    class impl;
    impl* pImpl;
};

} // namespace om
