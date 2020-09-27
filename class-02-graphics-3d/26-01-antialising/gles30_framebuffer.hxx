#pragma once
#include <cstdint>

namespace gles30
{
class texture;

class framebuffer
{
public:
    framebuffer(uint32_t width, uint32_t height);
    void color_attachment(texture&);
    bool is_complete();
    void bind();
    void unbind();

private:
    uint32_t fbo;
    uint32_t rbo; // for depth and stensil
};
} // namespace gles30
