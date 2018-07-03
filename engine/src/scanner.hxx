#pragma once

#include <string>

namespace om
{

struct file_info
{
    std::string  full_path = "";
    unsigned int size      = 0;
};

class file_list
{
public:
    unsigned int size();
    bool         empty();
    void         push();
    void         clear();

    class iterator;
    iterator begin();
    iterator end();

private:
    class impl;
    impl* pImpl;
};

struct scanner_report
{
    unsigned int scan_time      = 0;
    unsigned int total_files    = 0;
    unsigned int total_folders  = 0;
    bool         is_initialized = false;
    bool         scan_perfomed  = false;
};

class scanner
{
public:
    explicit scanner(const std::string& path);
    scanner(const scanner&);
    scanner& operator=(const scanner&);
    scanner(scanner&&);
    scanner& operator=(scanner&&);

    unsigned int get_file_size(std::string name); // may be replace with size_t?
    /**
     * Function return file size in bytes, if file exists. Otherwise -1.
     * Null is a valid return value. An input argument is a relative path
     * to the directory, where scanner was launched from. Invalid requests
     * such like path with empty string or without file extension (if present
     * in actual file) will also return -1;
     */

    bool is_file_exists(std::string name);

    /**
     * Function return true if file is exists on a given path. Invalid
     * requests such like path with empty string or without file extension
     * (if present in actual file) will return false;
     */

    const std::string get_file_path(std::string name);
    /**
     * Function return absolute path if file is exists on a given path.
     * Invalid requests such like path with empty string or without file
     * extension (if present in actual file) will return empty string. If
     * file couldn't be found the return value will be empty string too;
     */

    file_list get_all_files_with_extension(std::string extn, std::string path);
    /**
     * Function return a file_list container, which holds file_info structures
     * for given requirements. Empty string is valid as input parameter
     * and produce search in scanner's root directory. If one would specify
     * incorrect path (i.e. not valid or absent),function will return an empty
     * container. Empty string as file's extension argument will return a
     * non-empty container if path will contain files w/o extension.
     */

    file_list get_all_files_with_name(std::string name, std::string path);
    /**
     * Function return a file_list container, which holds file_info structures
     * for given requirements. Empty string is valid as input parameter
     * and produce search in scanner's root directory. If one would specify
     * incorrect path (i.e. not valid or absent),function will return an empty
     * container. Empty string for file's extension argument will produce search
     * for all files on given path.
     */

    scanner_report getReport();
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
