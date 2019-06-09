#include "gles30_model.hxx"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace gles30
{

void model::draw(shader& shader) const
{
    std::for_each(begin(meshes), end(meshes),
                  [&shader](const mesh& m) { m.draw(shader); });
}

void process_node(aiNode* node, const aiScene* scene, std::vector<mesh>& meshes,
                  const std::string& directory);
mesh process_mesh(aiMesh* mesh, const aiScene* scene,
                  const std::string& directory);
std::vector<texture*> load_material_textures(aiMaterial*        mat,
                                             aiTextureType      type,
                                             texture::uv_type   type_name,
                                             const std::string& directory);

void model::load_model(std::string_view path)
{
    std::string path_to_file{ path };

    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(
        path_to_file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        scene->mRootNode == nullptr)
    {
        std::stringstream ss;
        ss << "error: assimp loading model[" << path
           << "] failed: " << importer.GetErrorString() << std::endl;
        throw std::runtime_error(ss.str());
    }

    directory = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene, meshes, directory);
}

void process_node(aiNode* node, const aiScene* scene, std::vector<mesh>& meshes,
                  const std::string& directory)
{
    // process all the node's meshes (if any)
    auto begin_mesh = &node->mMeshes[0];
    auto end_mesh   = begin_mesh + node->mNumMeshes;

    std::for_each(begin_mesh, end_mesh, [&](auto mesh_index) {
        aiMesh*      assimp_mesh = scene->mMeshes[mesh_index];
        gles30::mesh mesh        = process_mesh(assimp_mesh, scene, directory);
        meshes.push_back(std::move(mesh));
    });

    auto beg_child = &node->mChildren[0];
    auto end_child = beg_child + node->mNumChildren;

    std::for_each(beg_child, end_child, [&](aiNode* sub_node) {
        process_node(sub_node, scene, meshes, directory);
    });
}

mesh process_mesh(aiMesh* mesh, const aiScene* scene,
                  const std::string& directory)
{
    std::vector<vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        vertex v;
        v.position.x = mesh->mVertices[i].x;
        v.position.y = mesh->mVertices[i].y;
        v.position.z = mesh->mVertices[i].z;

        v.normal.x = mesh->mNormals[i].x;
        v.normal.y = mesh->mNormals[i].y;
        v.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0])
        {
            v.uv.x = mesh->mTextureCoords[0]->x;
            v.uv.y = mesh->mTextureCoords[0]->y;
        }
        vertices.push_back(v);
    }

    // process indices
    std::vector<uint32_t> indices;

    indices.reserve(3 * mesh->mNumFaces);

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::vector<texture*> textures;
    // process material
    if (mesh->mMaterialIndex > 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<texture*> diffuse_maps =
            load_material_textures(material, aiTextureType_DIFFUSE,
                                   texture::uv_type::diffuse, directory);

        textures.insert(end(textures), begin(diffuse_maps), end(diffuse_maps));

        std::vector<texture*> specular_maps =
            load_material_textures(material, aiTextureType_SPECULAR,
                                   texture::uv_type::specular, directory);

        textures.insert(end(textures), begin(specular_maps),
                        end(specular_maps));
    }

    return gles30::mesh(std::move(vertices), std::move(indices),
                        std::move(textures));
}

std::vector<texture*> load_material_textures(aiMaterial*        mat,
                                             aiTextureType      type,
                                             texture::uv_type   type_name,
                                             const std::string& directory)
{
    std::vector<texture*> textures;
    size_t                count = mat->GetTextureCount(type);
    for (unsigned int i = 0; i < count; i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        texture* texture =
            new gles30::texture(fs::path{ directory + "/" + str.C_Str() });
        texture->set_type(type_name);
        textures.push_back(texture);
    }
    return textures;
}

} // namespace gles30
