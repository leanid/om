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
    std::for_each(begin(meshes),
                  end(meshes),
                  [&shader](const mesh& m) { m.draw(shader); });
}

void model::draw_instanced(shader&               shader,
                           size_t                instance_count,
                           std::function<void()> bind_custom_buffer) const
{
    std::for_each(
        begin(meshes),
        end(meshes),
        [&](const mesh& m)
        { m.draw_instanced(shader, instance_count, bind_custom_buffer); });
}

static void                  process_node(const aiNode*      node,
                                          const aiScene*     scene,
                                          std::vector<mesh>& meshes,
                                          const std::string& directory);
static mesh                  process_mesh(const aiMesh*      mesh,
                                          const aiScene*     scene,
                                          const std::string& directory);
static std::vector<texture*> load_material_textures(
    const aiMaterial*  mat,
    aiTextureType      type,
    texture::type      type_name,
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

static void process_node(const aiNode*      node,
                         const aiScene*     scene,
                         std::vector<mesh>& meshes,
                         const std::string& directory)
{
    // process all the node's meshes (if any)
    auto begin_mesh = &node->mMeshes[0];
    auto end_mesh   = begin_mesh + node->mNumMeshes;

    std::for_each(begin_mesh,
                  end_mesh,
                  [&](auto mesh_index)
                  {
                      const aiMesh* assimp_mesh = scene->mMeshes[mesh_index];
                      gles30::mesh  mesh =
                          process_mesh(assimp_mesh, scene, directory);
                      meshes.push_back(std::move(mesh));
                  });

    auto beg_child = &node->mChildren[0];
    auto end_child = beg_child + node->mNumChildren;

    std::for_each(beg_child,
                  end_child,
                  [&](aiNode* sub_node)
                  { process_node(sub_node, scene, meshes, directory); });
}

static mesh process_mesh(const aiMesh*      mesh,
                         const aiScene*     scene,
                         const std::string& directory)
{
    assert(mesh != nullptr);
    assert(scene != nullptr);

    std::vector<vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        vertex v;

        const aiVector3D& vec = mesh->mVertices[i];
        v.position.x          = vec.x;
        v.position.y          = vec.y;
        v.position.z          = vec.z;

        const aiVector3D& norm = mesh->mNormals[i];
        v.normal.x             = norm.x;
        v.normal.y             = norm.y;
        v.normal.z             = norm.z;

        vertices.push_back(v);
    }

    if (auto ptrToUV = mesh->mTextureCoords[0]; ptrToUV != nullptr)
    {
        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            const aiVector3D& uv      = ptrToUV[i];
            glm::vec2&        vert_uv = vertices[i].uv;
            vert_uv.x                 = uv.x;
            vert_uv.y                 = uv.y;
        }
    }

    // process indices
    std::vector<uint32_t> indices;

    indices.reserve(3 * mesh->mNumFaces);

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::vector<texture*> textures;
    // process material
    if (mesh->mMaterialIndex > 0)
    {
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        assert(material != nullptr);

        std::vector<texture*> diffuse_maps = load_material_textures(
            material, aiTextureType_DIFFUSE, texture::type::diffuse, directory);

        std::vector<texture*> specular_maps =
            load_material_textures(material,
                                   aiTextureType_SPECULAR,
                                   texture::type::specular,
                                   directory);

        textures.reserve(diffuse_maps.size() + specular_maps.size());
        textures.insert(end(textures), begin(diffuse_maps), end(diffuse_maps));
        textures.insert(
            end(textures), begin(specular_maps), end(specular_maps));
    }

    return gles30::mesh(
        std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<texture*> load_material_textures(const aiMaterial*  mat,
                                             aiTextureType      type,
                                             texture::type      type_name,
                                             const std::string& directory)
{
    std::vector<texture*> textures;
    size_t                count = mat->GetTextureCount(type);
    textures.reserve(count);
    for (unsigned int i = 0; i < count; i++)
    {
        aiString str;
        aiReturn isOk = mat->GetTexture(type, i, &str);
        assert(isOk == aiReturn_SUCCESS);

        std::filesystem::path texture_file_path{ directory };
        texture_file_path /= str.C_Str();

        texture* texture = new gles30::texture(texture_file_path, type_name);
        textures.push_back(texture);
    }
    return textures;
}

} // namespace gles30
