module;

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

export module gltf;

import std;

import vulkan_render;

namespace om::gltf
{
export om::vulkan::mesh load_model(std::filesystem::path path,
                                   om::vulkan::render&   render)
{
    auto buffer = fastgltf::GltfDataBuffer::FromPath(path);
    if (buffer.error() != fastgltf::Error::None)
    {
        throw std::runtime_error(
            std::string{ "can't load gltf file: " } + path.string() +
            " error: " +
            std::string{ fastgltf::getErrorMessage(buffer.error()) });
    }

    fastgltf::Parser parser;
    auto asset = parser.loadGltf(buffer.get(),
                                 path.parent_path(),
                                 fastgltf::Options::LoadExternalBuffers);
    if (asset.error() != fastgltf::Error::None)
    {
        throw std::runtime_error(
            std::string{ "can't parse gltf file: " } + path.string() +
            " error: " +
            std::string{ fastgltf::getErrorMessage(asset.error()) });
    }

    std::unordered_map<om::vulkan::vertex, std::uint32_t> unique_vertexes;

    std::vector<om::vulkan::vertex> vertices;
    std::vector<std::uint32_t>      indices;

    for (const auto& mesh : asset->meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            if (primitive.type != fastgltf::PrimitiveType::Triangles)
            {
                continue;
            }

            const auto* position_it = primitive.findAttribute("POSITION");
            if (position_it == primitive.attributes.end())
            {
                continue;
            }
            const auto& position_accessor =
                asset->accessors[position_it->accessorIndex];

            std::vector<fastgltf::math::fvec3> positions(
                position_accessor.count);
            fastgltf::copyFromAccessor<fastgltf::math::fvec3>(
                asset.get(), position_accessor, positions.data());

            // glTF uv origin is top-left - the same convention the obj
            // version produced with "1.0f - v", so use uv as-is
            std::vector<fastgltf::math::fvec2> texcoords(
                position_accessor.count);
            const auto* texcoord_it = primitive.findAttribute("TEXCOORD_0");
            if (texcoord_it != primitive.attributes.end())
            {
                const auto& texcoord_accessor =
                    asset->accessors[texcoord_it->accessorIndex];
                fastgltf::copyFromAccessor<fastgltf::math::fvec2>(
                    asset.get(), texcoord_accessor, texcoords.data());
            }

            std::vector<std::uint32_t> local_indices;
            if (primitive.indicesAccessor.has_value())
            {
                const auto& index_accessor =
                    asset->accessors[*primitive.indicesAccessor];
                local_indices.resize(index_accessor.count);
                fastgltf::copyFromAccessor<std::uint32_t>(
                    asset.get(), index_accessor, local_indices.data());
            }
            else
            {
                local_indices.resize(position_accessor.count);
                std::iota(local_indices.begin(), local_indices.end(), 0u);
            }

            for (const std::uint32_t index : local_indices)
            {
                const auto&        pos = positions[index];
                const auto&        uv  = texcoords[index];
                om::vulkan::vertex vertex{ // pos
                                           { pos.x(), pos.y(), pos.z() },
                                           // col
                                           { 1.0f, 1.0f, 1.0f },
                                           // tex
                                           { uv.x(), uv.y() }
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
    }

    return om::vulkan::mesh(
        std::span{ vertices }, std::span{ indices }, render, "viking_home");
}
} // namespace om::gltf
