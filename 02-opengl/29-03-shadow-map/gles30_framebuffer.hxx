#pragma once
#include <cstdint>

#include "opengles30.hxx"

namespace gles30
{
class texture;

enum class generate_render_object
{
    no,
    yes
};

class framebuffer
{
public:
    framebuffer(uint32_t               width,
                uint32_t               height,
                generate_render_object ro_value = generate_render_object::yes,
                multisampling          ms_value = multisampling::disable,
                uint32_t               multisample_count = 1);
    void        color_attachment(texture&);
    void        depth_attachment(texture&);
    void        disable_draw_buffer();
    void        disable_read_buffer();
    bool        is_complete();
    std::string get_status_message();
    void        bind();
    void        unbind();

    void blit_to_framebuffer(framebuffer& destenasion,
                             const rect&  src,
                             const rect&  dst,
                             uint32_t     mask_canals,
                             filter       filtering);

private:
    uint32_t fbo;
    uint32_t rbo; // for depth and stensil
};
} // namespace gles30
