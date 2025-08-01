#pragma once

#include <cstdint>
#include <string>

namespace om::vulkan
{
struct args_parser
{
    args_parser(int argc, char** argv);

    std::string   help;
    std::uint32_t vulkan_version_major;
    std::uint32_t vulkan_version_minor;
    bool          verbose;
    bool          validation_layer;
    bool          debug_callback;
};
} // namespace om::vulkan
