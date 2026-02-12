#include <array>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <type_traits>
#include "fps_camera.hxx"
#include "gles30_framebuffer.hxx"
#include "gles30_model.hxx"
#include "gles30_shader.hxx"
#include "gles30_texture.hxx"
#include "opengles30.hxx"
#include "properties_reader.hxx"

#include "res/runtime.properties.hxx"

static fps_camera camera;

const std::array<std::pair<std::string_view, int>, 8> z_buf_operations{
    { { "always", GL_ALWAYS },
      { "never", GL_NEVER },
      { "less", GL_LESS },
      { "equal", GL_GEQUAL },
      { "lequal", GL_LEQUAL },
      { "greater", GL_GREATER },
      { "notequal", GL_NOTEQUAL },
      { "gequal", GL_GEQUAL } }
};

const std::array<std::pair<std::string_view, int>, 8> stensil_operations{
    { { "keep", GL_KEEP },       /// The currently stored stencil value is kept.
      { "zero", GL_ZERO },       /// The stencil value is set to 0.
      { "replace", GL_REPLACE }, /// The stencil value is replaced with ref
      { "incr", GL_INCR },       /// The stencil value is increased by 1
      { "incr_wrap", GL_INCR_WRAP }, /// same as above but goto 0
      { "decr", GL_DECR },           /// decremented by 1
      { "decr_wrap", GL_DECR_WRAP }, /// same as above but restart with max FF
      { "invert", GL_INVERT } } /// Bitwise inverts the current stencil value.
};

std::array<std::filesystem::path, 6> faces{
    "res/skybox/right.jpg",  "res/skybox/left.jpg",  "res/skybox/top.jpg",
    "res/skybox/bottom.jpg", "res/skybox/front.jpg", "res/skybox/back.jpg"
};

int get_gl_constant(
    const std::array<std::pair<std::string_view, int>, 8>& operations,
    std::string_view                                       name)
{
    auto it = std::find_if(begin(operations),
                           end(operations),
                           [&name](const std::pair<std::string_view, int>& p)
                           { return p.first == name; });
    if (it == end(operations))
    {
        throw std::out_of_range(std::string("operation not found: ") +
                                std::string(name));
    }
    return it->second;
}

int get_z_buf_operation(std::string_view name)
{
    return get_gl_constant(z_buf_operations, name);
}

int get_stensil_operation(std::string_view name)
{
    return get_gl_constant(stensil_operations, name);
}

#pragma pack(push, 4)
struct context_parameters
{
    std::string_view name{};
    int32_t          major_version = 0;
    int32_t          minor_version = 0;
    int32_t          profile_type  = 0;
};

#pragma pack(pop)

bool operator==(const context_parameters& l, const context_parameters& r)
{
    return std::string_view(l.name) == r.name &&
           l.major_version == r.major_version && l.minor_version &&
           r.minor_version && l.profile_type && r.profile_type;
}

bool operator!=(const context_parameters& l, const context_parameters& r)
{
    return !(l == r);
}

std::ostream& operator<<(std::ostream& out, const context_parameters& params)
{
    out << params.name << ' ' << params.major_version << '.'
        << params.minor_version;
    return out;
}

void print_view_port()
{
    using namespace std;

    GLint view_port[4];
    glGetIntegerv(GL_VIEWPORT, view_port);

    clog << "view port is: x=" << view_port[0] << " y=" << view_port[1]
         << " w=" << view_port[2] << " h=" << view_port[3] << endl;
}

extern const float cube_vertices[36 * 8];
extern const float plane_vertices[6 * 8];
extern const float transparent_vert[6 * 8];
extern const float fullscreen_vertices[6 * 8];

enum class render_options
{
    only_pro_view_model,
    set_near_far_etc,
    no_matrix,
    only_pro_view
};

void render_mesh(gles30::shader&          shader,
                 const fps_camera&        camera,
                 const gles30::mesh&      mesh,
                 glm::vec3                position,
                 float                    scale,
                 const properties_reader& properties,
                 const render_options     options)
{
    shader.use();
    if (options == render_options::set_near_far_etc)
    {
        linear_z_buffer = properties.get_bool("linear_z_buffer");
        show_z_buffer   = properties.get_bool("show_z_buffer");
        z_near          = properties.get_float("z_near");
        z_far           = properties.get_float("z_far");
        shader.set_uniform("show_z_buffer", show_z_buffer);
        shader.set_uniform("linear_z_buffer", linear_z_buffer);
        shader.set_uniform("z_near", z_near);
        shader.set_uniform("z_far", z_far);
    }
    if (options == render_options::only_pro_view_model ||
        options == render_options::set_near_far_etc)
    {
        shader.set_uniform("projection", camera.projection_matrix());
        shader.set_uniform("view", camera.view_matrix());

        {
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, position);
            model           = glm::scale(model, glm::vec3(scale));
            shader.set_uniform("model", model);
            mesh.draw(shader);
        }
    }
    if (options == render_options::only_pro_view)
    {
        shader.set_uniform("projection", camera.projection_matrix());
        // remove translation part of matrix, we only need direction
        shader.set_uniform("view", glm::mat4(glm::mat3(camera.view_matrix())));
        mesh.draw(shader);
    }
    else if (options == render_options::no_matrix)
    {
        mesh.draw(shader);
    }
}

static void destroy_opengl_context(void* ptr)
{
    // for debug check
    SDL_GL_DestroyContext(ptr);
}

static const char* source_to_strv(GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
    }
    return "unknown";
}

static const char* type_to_strv(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "POP_GROUP";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
    }
    return "unknown";
}

static const char* severity_to_strv(GLenum severity)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
    }
    return "unknown";
}

// 30Kb on my system, too much for stack
static std::array<char, GL_MAX_DEBUG_MESSAGE_LENGTH> local_log_buff;

static void APIENTRY
callback_opengl_debug(GLenum                       source,
                      GLenum                       type,
                      GLuint                       id,
                      GLenum                       severity,
                      GLsizei                      length,
                      const GLchar*                message,
                      [[maybe_unused]] const void* userParam)
{
    // The memory formessageis owned and managed by the GL, and should onlybe
    // considered valid for the duration of the function call.The behavior of
    // calling any GL or window system function from within thecallback function
    // is undefined and may lead to program termination.Care must also be taken
    // in securing debug callbacks for use with asynchronousdebug output by
    // multi-threaded GL implementations.  Section 18.8 describes thisin further
    // detail.

    auto& buff{ local_log_buff };
    int   num_chars = std::snprintf(buff.data(),
                                  buff.size(),
                                  "%s %s %d %s %.*s\n",
                                  source_to_strv(source),
                                  type_to_strv(type),
                                  id,
                                  severity_to_strv(severity),
                                  length,
                                  message);

    if (num_chars > 0)
    {
        // TODO use https://en.cppreference.com/w/cpp/io/basic_osyncstream
        // to fix possible data races
        // now we use GL_DEBUG_OUTPUT_SYNCHRONOUS to garantie call in main
        // thread
        std::cerr.write(buff.data(), num_chars);
    }
}

[[nodiscard]] std::unique_ptr<void, void (*)(void*)> create_opengl_context(
    SDL_Window* window)
{
    using namespace std;
    context_parameters ask_context;

    string_view platform_name = SDL_GetPlatform();

    const array<string_view, 3> desktop_platforms{ "Windows",
                                                   "Mac OS X",
                                                   "Linux" };

    auto it =
        find(begin(desktop_platforms), end(desktop_platforms), platform_name);

    if (it != end(desktop_platforms))
    {
        // we want OpenGL Core 3.3 context
        ask_context.name          = "OpenGL Core";
        ask_context.major_version = 3;
        ask_context.minor_version = 3;
        ask_context.profile_type  = SDL_GL_CONTEXT_PROFILE_CORE;
    }
    else
    {
        // we want OpenGL ES 3.2 context
        ask_context.name          = "OpenGL ES";
        ask_context.major_version = 3;
        ask_context.minor_version = 2;
        ask_context.profile_type  = SDL_GL_CONTEXT_PROFILE_ES;
    }

    int r;
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            ask_context.profile_type);
    SDL_assert_always(r);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                            ask_context.major_version);
    SDL_assert_always(r);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                            ask_context.minor_version);
    SDL_assert_always(r);

    using gl_context_t = std::unique_ptr<std::remove_pointer_t<SDL_GLContext>,
                                         decltype(&SDL_GL_DestroyContext)>;
    gl_context_t gl_context(SDL_GL_CreateContext(window),
                                                 destroy_opengl_context);
    if (nullptr == gl_context)
    {
        clog << "Failed to create: " << ask_context
             << " error: " << SDL_GetError() << endl;
        SDL_Quit();
        throw std::runtime_error("error: can't create opengl context");
    }

    context_parameters got_context = ask_context;

    int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                                     &got_context.major_version);
    assert(result);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                 &got_context.minor_version);
    assert(result);
    int got_depth_size = 0;
    result = SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &got_depth_size);
    assert(result);
    int stensil_size = 0;
    result           = SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stensil_size);
    assert(result);

    if (ask_context != got_context)
    {
        clog << "Ask for " << ask_context << endl;
        clog << "Receive " << got_context << endl;
    }

    initialize_opengles_3_2();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(callback_opengl_debug, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    if (got_context.name == "OpenGL Core")
    {
// we have to emulate OpenGL ES 3.2 so enable gl_PointSize
#define GL_PROGRAM_POINT_SIZE 0x8642
        glEnable(GL_PROGRAM_POINT_SIZE);
#undef GL_PROGRAM_POINT_SIZE
    }

    return gl_context;
};

void pull_system_events(bool& continue_loop, int& current_effect)
{
    using namespace std;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (SDL_EVENT_FINGER_DOWN == event.type)
        {
            continue_loop = false;
            break;
        }
        else if (SDL_EVENT_QUIT == event.type)
        {
            continue_loop = false;
            break;
        }
        else if (SDL_EVENT_MOUSE_MOTION == event.type)
        {
            const float sensivity   = 0.05f;
            const float delta_yaw   = event.motion.xrel * sensivity;
            const float delta_pitch = -1.f * event.motion.yrel * sensivity;
            camera.rotate(delta_yaw, delta_pitch);
        }
        else if (SDL_EVENT_MOUSE_WHEEL == event.type)
        {
            camera.zoom(-event.wheel.y);
        }
        else if (SDL_EVENT_KEY_UP == event.type)
        {
            if (event.key.key == SDLK_0)
            {
                current_effect = 5;
            }
            else if (event.key.key == SDLK_1)
            {
                current_effect = 1;
            }
            else if (event.key.key == SDLK_2)
            {
                current_effect = 2;
            }
            else if (event.key.key == SDLK_3)
            {
                current_effect = 3;
            }
            else if (event.key.key == SDLK_4)
            {
                current_effect = 4;
            }
            else if (event.key.key == SDLK_5)
            {
                if (!SDL_SetWindowRelativeMouseMode(window.get(), true))
                {
                    throw std::runtime_error(SDL_GetError());
                }
            }
            else if (event.key.key == SDLK_6)
            {
                if (!SDL_SetWindowRelativeMouseMode(window.get(), false))
                {
                    throw std::runtime_error(SDL_GetError());
                }
            }
        }
        else if (SDL_EVENT_WINDOW_RESIZED == event.type)
        {
            clog << "windows resized: " << event.window.data1 << ' '
                 << event.window.data2 << ' ';
            // play with it to understand OpenGL origin point
            // for window screen coordinate system
            screen_width  = event.window.data1;
            screen_height = event.window.data2;
            screen_aspect = screen_width / screen_height;
            camera.aspect(screen_aspect);
            glViewport(0, 0, event.window.data1, event.window.data2);
            print_view_port();
        }
    }
}

float update_delta_time(float& lastFrame)
{
    float currentFrame = SDL_GetTicks() * 0.001f; // seconds
    float deltaTime    = currentFrame - lastFrame;
    lastFrame          = currentFrame;
    return deltaTime;
}

void clear_back_buffer(const glm::vec3 clear_color)
{
    float red   = clear_color.r;
    float green = clear_color.g;
    float blue  = clear_color.b;
    float alpha = 0.f;

    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static void destroy_window(SDL_Window* ptr)
{
    // for debug check
    SDL_DestroyWindow(ptr);
    // do not call SDL_Quit() if you still has window or context
    // sdl destroy OpenGL context with window
    SDL_Quit();
}

std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> create_window(
    const properties_reader& properties)
{
    using namespace std;
    const bool init_result = SDL_Init(SDL_INIT_VIDEO);
    if (!init_result)
    {
        std::string err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        throw std::runtime_error(err_message);
    }

    title         = properties.get_string("title");
    screen_width  = properties.get_float("screen_width");
    screen_height = properties.get_float("screen_height");

    // we have to set DEPTH_SIZE before creating window!
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    // https://gamedev.stackexchange.com/questions/120644/is-glxinfo-saying-that-the-980-gtx-doesnt-support-a-32-bit-depth-buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), static_cast<int>(screen_width), static_cast<int>(screen_height), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE),
        destroy_window);

    if (window.get() == nullptr)
    {
        std::string err_message = SDL_GetError();
        clog << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        throw std::runtime_error(err_message);
    }

    return window;
}

gles30::mesh create_mesh(const float*                  vertices,
                         size_t                        count_vert,
                         std::vector<gles30::texture*> textures)
{
    using namespace std;
    vector<gles30::vertex> vert;
    vert.reserve(count_vert);
    for (size_t index = 0; index < count_vert; index++)
    {
        gles30::vertex v;
        size_t         i = index * 8;
        v.position.x     = vertices[i + 0];
        v.position.y     = vertices[i + 1];
        v.position.z     = vertices[i + 2];

        v.normal.x = vertices[i + 3];
        v.normal.y = vertices[i + 4];
        v.normal.z = vertices[i + 5];

        v.uv.x = vertices[i + 6];
        v.uv.y = vertices[i + 7];

        vert.push_back(v);
    }
    vector<uint32_t> indexes(count_vert);
    std::iota(begin(indexes), end(indexes), 0);

    return gles30::mesh(
        std::move(vert), std::move(indexes), std::move(textures));
}

void create_camera(const properties_reader& properties)
{
    cam_pos = properties.get_vec3("cam_pos");
    cam_dir = properties.get_vec3("cam_dir");

    camera = fps_camera(cam_pos,
                        cam_dir,
                        /*up*/ { 0, 1, 0 });

    fovy = properties.get_float("fovy");
    camera.fovy(fovy);
    screen_aspect = properties.get_float("screen_aspect");
    camera.aspect(screen_aspect);
}

struct scene
{
    scene();
    void render(float delta_time);
    void render_fullscreen_quad();

    properties_reader properties;

    std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window;
    std::unique_ptr<void, void (*)(void*)>             context;

    gles30::shader cube_shader;

    gles30::texture tex_marble;
    gles30::texture tex_metal;
    gles30::texture tex_grass;
    gles30::texture tex_window;
    gles30::texture tex_color_buffer;
    gles30::texture tex_cubemap;

    gles30::mesh cube_mesh;
};

scene::scene()
    : properties("res/runtime.properties.hxx")
    , window{ create_window(properties) }
    , context{ create_opengl_context(window.get()) }
    , cube_shader("res/cube.vsh", "res/cube.fsh")
    , tex_marble("res/marble.jpg", gles30::texture::type::diffuse)
    , tex_metal("res/metal.png", gles30::texture::type::diffuse)
    , tex_grass("res/grass.png",
                gles30::texture::type::diffuse,
                gles30::texture::opt::no_flip)
    , tex_window("res/blending_transparent_window.png",
                 gles30::texture::type::diffuse,
                 gles30::texture::opt::no_flip)
    , tex_color_buffer(gles30::texture::type::diffuse,
                       properties.get_float("screen_width"),
                       properties.get_float("screen_height"))
    , tex_cubemap(faces, gles30::texture::opt::no_flip)
    , cube_mesh{ create_mesh(
          cube_vertices, sizeof(cube_vertices) / 4 / 8, { &tex_metal }) }
{
    create_camera(properties);
    cube_mesh.set_primitive_type(gles30::primitive::triangles);
}

void scene::render(float delta_time)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_PROGRAM_POINT_SIZE); only for DESKTOP GL

    camera.move_using_keyboard_wasd(delta_time);

    clear_back_buffer(properties.get_vec3("clear_color"));

    float scale = 1.0f;

    cube_shader.use();
    cube_shader.set_uniform("screen_size",
                            glm::vec2(screen_width, screen_height));
    render_mesh(cube_shader,
                camera,
                cube_mesh,
                glm::vec3(0.0f, 0.0f, 0.0f),
                scale,
                properties,
                render_options::only_pro_view_model);
}

int main(int /*argc*/, char* /*argv*/[])
{
    {
        scene scene;

        float last_frame_time      = 0.0f; // Time of last frame
        int   current_post_process = 0;

        for (bool continue_loop = true; continue_loop;)
        {
            float delta_time = update_delta_time(last_frame_time);

            scene.properties.update_changes();

            pull_system_events(continue_loop, current_post_process);

            scene.render(delta_time);

            SDL_GL_SwapWindow(scene.window.get());
        }
    }

    return 0;
}

// clang-format off
const float cube_vertices[36 * 8] =
{
    // positions         // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

const float plane_vertices[6 * 8] =
{
    // positions                            // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    5.0f, -0.5f,  5.0f, 0.0f, 0.0f, 0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f, 0.0f, 0.0f, 0.0f,  0.0f, 2.0f,

    5.0f, -0.5f,  5.0f, 0.0f, 0.0f, 0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f, 0.0f, 0.0f, 0.0f,  0.0f, 2.0f,
    5.0f, -0.5f, -5.0f, 0.0f, 0.0f, 0.0f,  2.0f, 2.0f
};

const float transparent_vert[6 * 8] =
{
    // positions        // normal          // texture Coords (swapped y coordinates because texture is flipped upside down)
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    0.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  1.0f,
    1.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  1.0f,

    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    1.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  1.0f,
    1.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f
};
const float fullscreen_vertices[6 * 8] {
    // positions       // normal       // texCoords
    -1.0f,  1.0f, 0.f, 0.f, 0.f, 0.f,  0.0f, 1.0f,
    -1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f,  0.0f, 0.0f,
    1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f,  1.0f, 0.0f,

    -1.0f,  1.0f, 0.f, 0.f, 0.f, 0.f,  0.0f, 1.0f,
    1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f,  1.0f, 0.0f,
    1.0f,  1.0f, 0.f, 0.f, 0.f, 0.f,  1.0f, 1.0f
};
// clang-format on
