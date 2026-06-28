module;

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

export module tinyobj;

import std;

import vulkan_render;

namespace om::tinyobj
{
export om::vulkan::mesh load_model(std::filesystem::path path,
                                   om::vulkan::render&   render)
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

    std::vector<om::vulkan::vertex> vertices;
    std::vector<std::uint32_t>      indices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            om::vulkan::vertex vertex{
                // pos
                { attrib.vertices[3 * index.vertex_index + 0],
                  attrib.vertices[3 * index.vertex_index + 1],
                  attrib.vertices[3 * index.vertex_index + 2] },
                // col
                { 1.0f, 1.0f, 1.0f },
                // tex
                { attrib.texcoords[2 * index.texcoord_index + 0],
                  attrib.texcoords[2 * index.texcoord_index + 1] }
            };

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    return om::vulkan::mesh(
        std::span{ vertices }, std::span{ indices }, render, "viking_home");
}
} // namespace om::tinyobj
