#include "gles30_framebuffer.hxx"

#include "gles30_texture.hxx"
#include "opengles30.hxx"

namespace gles30
{
framebuffer::framebuffer(uint32_t               width,
                         uint32_t               height,
                         generate_render_object ro_value,
                         multisampling          ms_value,
                         uint32_t               multisample_count)
    : fbo{}
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if (generate_render_object::yes == ro_value)
    {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        if (multisampling::disable == ms_value)
        {
            glRenderbufferStorage(
                GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        }
        else
        {
            glRenderbufferStorageMultisample(
                GL_RENDERBUFFER,
                multisample_count, // 1, 2, 4 - values
                GL_DEPTH24_STENCIL8,
                width,
                height);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }
}

void framebuffer::color_attachment(texture& tex)
{
    bind();
    tex.bind();
    int level = 0; // Specifies the mipmap level of texture to attach
    if (tex.get_type() == texture::type::multisample2d)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D_MULTISAMPLE,
                               tex.texture_id,
                               level);
    }
    else
    {

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               tex.texture_id,
                               level);
    }
}

bool framebuffer::is_complete()
{
    bind();
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

std::string framebuffer::get_status_message()
{
    bind();
    const int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return "complete";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "incomplete attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            return "incomplete_dimensions";
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            return "incomplete_layer_targets";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "incomplete_missing_attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return "incomplete_multisample";
        default:
        {
            std::string err = "error: can't convert framebuffer status code: " +
                              std::to_string(status);
            return err;
        }
    }
}

void framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void framebuffer::blit_to_framebuffer(framebuffer& destenasion,
                                      const rect&  src,
                                      const rect&  dst,
                                      uint32_t     mask_canals,
                                      filter       filtering)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destenasion.fbo);
    int filt = to_gl_filter_enum(filtering);
    glBlitFramebuffer(src.x0,
                      src.y0,
                      src.x1,
                      src.y1,
                      dst.x0,
                      dst.y0,
                      dst.x1,
                      dst.y1,
                      mask_canals,
                      filt);
}

} // namespace gles30
