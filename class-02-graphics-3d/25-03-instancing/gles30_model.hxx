#pragma once

#include "gles30_mesh.hxx"

namespace gles30
{

class model
{
public:
    explicit model(std::string_view path) { load_model(path); }
    void draw(shader& shader) const;
    void draw_instanced(shader&               shader,
                        size_t                instance_count,
                        std::function<void()> bind_custom_buffer) const;

private:
    void load_model(std::string_view path);

    std::vector<mesh> meshes;
    std::string       directory;
};

} // namespace gles30
