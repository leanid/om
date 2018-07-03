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

    file_info get_file_info(std::string name);

    unsigned int get_file_size(std::string name);

    bool is_file_exists(std::string name);

    const std::string get_file_path(std::string name);

    file_list get_all_files_with_extension(std::string extn, std::string path);

    file_list get_all_files_with_name(std::string name, std::string path);

    scanner_report getReport();

    ~scanner();

private:
    class impl;
    impl* pImpl;
};

} // namespace om
