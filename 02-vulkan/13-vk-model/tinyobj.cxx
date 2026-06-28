module;

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

export module tinyobj;

import std;

import vulkan_render;

namespace om::tinyobj
{
export void load_model()
{
    using namespace ::tinyobj;
    attrib_t                attrib;
    std::vector<shape_t>    shapes;
    std::vector<material_t> materials;
    std::string             warn, err;

    if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, "not_exist.obj"))
    {
        throw std::runtime_error(warn + err);
    }
}
} // namespace om::tinyobj
