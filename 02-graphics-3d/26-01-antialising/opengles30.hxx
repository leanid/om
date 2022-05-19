#pragma once

#include <glad/glad.h>

#include <array>
#include <iosfwd>
#include <string_view>

void windows_make_process_dpi_aware() noexcept(false);
void initialize_opengles_3_2() noexcept(false);
bool is_desktop();
bool is_mobile();

void APIENTRY callback_opengl_debug(GLenum                       source,
                                    GLenum                       type,
                                    GLuint                       id,
                                    GLenum                       severity,
                                    GLsizei                      length,
                                    const GLchar*                message,
                                    [[maybe_unused]] const void* userParam);

void print_view_port();

#pragma pack(push, 4)
struct context_parameters
{
    std::string_view name{};
    int32_t          major_version = 0;
    int32_t          minor_version = 0;
    int32_t          profile_type  = 0;
};
#pragma pack(pop)

std::ostream& operator<<(std::ostream& out, const context_parameters& params);
bool operator==(const context_parameters& l, const context_parameters& r);
bool operator!=(const context_parameters& l, const context_parameters& r);
