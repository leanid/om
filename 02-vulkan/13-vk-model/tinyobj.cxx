module;

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

export module tinyobj;

import std;

import vulkan_render;

namespace om::tinyobj
{
export void load_model(std::filesystem::path path)
{
    using namespace ::tinyobj;
    attrib_t                attrib;
    std::vector<shape_t>    shapes;
    std::vector<material_t> materials;
    std::string             warn, err;

    if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        throw std::runtime_error(warn + err);
    }
}
} // namespace om::tinyobj
