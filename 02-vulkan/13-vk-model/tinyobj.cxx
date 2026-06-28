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

    std::unordered_map<om::vulkan::vertex, std::uint32_t> unique_vertexes;

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
                // tex Y direction in Vulkan from up to down
                { attrib.texcoords[2 * index.texcoord_index + 0],
                  1.0f - attrib.texcoords[2 * index.texcoord_index + 1] }
            };

            auto [it, inserted] = unique_vertexes.insert(
                { vertex, static_cast<std::uint32_t>(vertices.size()) });
            if (inserted)
            {
                vertices.push_back(vertex);
            }

            indices.push_back(it->second);
        }
    }

    return om::vulkan::mesh(
        std::span{ vertices }, std::span{ indices }, render, "viking_home");
}
} // namespace om::tinyobj
