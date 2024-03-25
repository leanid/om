#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include <glm/vec3.hpp>

class properties_reader
{
public:
    explicit properties_reader(const std::filesystem::path& path);
    properties_reader(const properties_reader&)            = delete;
    properties_reader& operator=(const properties_reader&) = delete;
    ~properties_reader();

    void update_changes();

    [[nodiscard]] const std::string& get_string(std::string_view name) const
        noexcept(false);
    [[nodiscard]] float get_float(std::string_view name) const noexcept(false);
    [[nodiscard]] const glm::vec3& get_vec3(std::string_view name) const
        noexcept(false);
    [[nodiscard]] bool get_bool(std::string_view name) const noexcept(false);

private:
    class impl;
    std::unique_ptr<impl> ptr;
};
