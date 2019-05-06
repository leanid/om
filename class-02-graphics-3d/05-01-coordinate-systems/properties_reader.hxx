#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include <glm/vec3.hpp>

#ifdef _MSC_VER
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class properties_reader
{
public:
    explicit properties_reader(const fs::path& path);
    properties_reader(const properties_reader&) = delete;
    properties_reader& operator=(const properties_reader&) = delete;
    ~properties_reader();

    void update_changes();

    const std::string& get_string(std::string_view name) const noexcept(false);
    float              get_float(std::string_view name) const noexcept(false);
    const glm::vec3&   get_vec3(std::string_view name) const noexcept(false);
    bool               get_bool(std::string_view name) const noexcept(false);

private:
    class impl;
    std::unique_ptr<impl> ptr;
};
