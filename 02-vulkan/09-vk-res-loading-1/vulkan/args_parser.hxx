#pragma once

#include <cstdint>
#include <string>

namespace om::vulkan
{
struct args_parser
{
    args_parser(int argc, char** argv);

    std::string   help                 = "";
    std::uint32_t vulkan_version_major = 0;
    std::uint32_t vulkan_version_minor = 0;
    bool          verbose              = false;
    bool          validation_layer     = false;
    bool          debug_callback       = false;
};
} // namespace om::vulkan
