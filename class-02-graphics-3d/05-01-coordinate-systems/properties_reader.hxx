#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#ifdef _MSC_VER
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class properties_reader
{
public:
    explicit properties_reader(const fs::path& path);

    void update_changes();

    std::string_view get_string(std::string_view name) const noexcept(false);
    std::int32_t     get_int(std::string_view name) const noexcept(false);
    float            get_float(std::string_view name) const noexcept(false);

    properties_reader(const properties_reader&) = delete;
    properties_reader& operator=(const properties_reader&) = delete;

    ~properties_reader();

private:
    class impl;
    std::unique_ptr<impl> ptr;
};
