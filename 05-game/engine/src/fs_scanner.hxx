#pragma once

#ifdef _WIN32
#define SCNR_EXP __declspec(dllexport)
#else
#define SCNR_EXP
#endif

#include <string>
#include <vector>

namespace om
{

struct file_info
{
    std::u8string abs_path;
    size_t        size = 0;
};

struct scanner_report
{
    size_t    scan_time     = 0;
    size_t    total_files   = 0;
    size_t    total_folders = 0;
    bool      initialized   = false;
    std::byte padding[7]    = {};
};

class SCNR_EXP scanner final
{
public:
    scanner()                          = delete;
    scanner(const scanner&)            = delete;
    scanner& operator=(const scanner&) = delete;

    scanner(scanner&&) noexcept;
    scanner& operator=(scanner&&) noexcept;

    explicit scanner(std::u8string_view path);

    [[nodiscard]] size_t get_file_size(std::u8string_view name) const;

    // Function return file size in bytes, if file exists. Otherwise -1.
    // Null is a valid return value. The "name" parameter must be
    // a relative path, where scanner was launched from, with
    // and extension if presents. Invalid requests like empty string
    // or incorrect (non-exist) path will also return -1;

    [[nodiscard]] bool is_file_exists(std::u8string_view path) const;

    // Function returns true if file exists on a given path. Invalid
    // requests like empty or incorrect path or name will return false;

    [[nodiscard]] std::vector<file_info> get_files_with_extension(
        std::u8string_view path, std::u8string_view ext) const;

    // Function return a file_list container, which holds file_info
    // structures for given requirements. List will be empty if
    // nothing was found. Empty path produces search in scanner's
    // root directory.  Empty extension makes search for files
    // w/o extension. Incorrect parameters (i.e. invalid path
    // or incorrect extension) will return an empty container.

    [[nodiscard]] std::vector<file_info> get_files_with_name(
        std::u8string_view path, std::u8string_view name) const;

    // Function return a file_list container, which holds file_info
    // structures for given requirements. List will be empty if
    // nothing was found. Empty path produces search in scanner's
    // root directory.  Empty name is an incorrect value.
    // Incorrect parameters will return an empty container.

    [[nodiscard]] std::vector<file_info> get_files(
        std::u8string_view path) const;

    // Function return a file_list container, which holds file_info
    // structure for all files in a  given path. List will be empty
    // if given path's directory doesn't contain any files.
    // Empty path produces search in scanner's root directory.
    // Non-exist path will return an empty container.

    [[nodiscard]] std::vector<file_info> get_all_files() const;

    // Function return a file_list container, which holds file_info
    // structure for all files that scanner found in all directories.

    [[nodiscard]] scanner_report get_report() const;

    // Function return a scanner_report structure, which contains
    // information about scanner internal values and states.

    ~scanner();

private:
    class impl;
    impl* pImpl;
};

} // namespace om
