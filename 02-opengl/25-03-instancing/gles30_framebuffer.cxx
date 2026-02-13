#include "gles30_framebuffer.hxx"

#include "gles30_texture.hxx"
#include "opengles30.hxx"

namespace gles30
{
framebuffer::framebuffer(uint32_t width, uint32_t height)
    : fbo{}
{
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH24_STENCIL8,
                          static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void framebuffer::color_attachment(texture& tex)
{
    bind();
    tex.bind();
    int level = 0; // Specifies the mipmap level of texture to attach
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           tex.texture_id,
                           level);
}

bool framebuffer::is_complete()
{
    bind();
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
} // namespace gles30
