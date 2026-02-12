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

extern const std::array<float, 6 * 8> quad_virtices;
extern const std::array<float, 36 * 8> cube_vertices;
extern const std::array<float, 6 * 8> plane_vertices;

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

static bool destroy_opengl_context(SDL_GLContext ptr)
{
    return SDL_GL_DestroyContext(ptr);
}

[[nodiscard]] std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, decltype(&SDL_GL_DestroyContext)> create_opengl_context(
    SDL_Window* window)
{
    using namespace std;
    using namespace gles30;
    context_parameters ask_context;

    if (is_desktop())
    {
        // for MacOSX OpenGL version: https://support.apple.com/en-us/HT202823
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

    clog << "Ask for " << ask_context << endl;
    clog << "Receive " << got_context << endl;

    initialize_opengles_3_2();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    if (is_desktop())
    {

#define GL_MULTISAMPLE 32925      // or 0x809D
        glEnable(GL_MULTISAMPLE); // not working in GLES3.0
#undef GL_MULTISAMPLE
    }
    else
    {
        // TODO
    }

    glEnable(GL_DEBUG_OUTPUT);
    // on MacOS no such functional
    if (glDebugMessageCallback != nullptr)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(callback_opengl_debug, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    if (is_desktop())
    {
// we have to emulate OpenGL ES 3.2 so enable gl_PointSize
#define GL_PROGRAM_POINT_SIZE 0x8642
        glEnable(GL_PROGRAM_POINT_SIZE);
#undef GL_PROGRAM_POINT_SIZE
    }

    return gl_context;
};

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
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
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
    const properties_reader& properties, gles30::multisampling multisampling)
{
    using namespace std;
    using namespace gles30;
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

    if (is_desktop())
    {
        // this works on desctop OpenGL
        if (multisampling::enable == multisampling)
        {
            int r = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_assert_always(r);
            r = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
            SDL_assert_always(r);
        }
    }
    else
    {
        // TODO
    }

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), static_cast<int>(screen_width), static_cast<int>(screen_height), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*|
                             SDL_WINDOW_ALLOW_HIGHDPI*/ /*|
                             SDL_WINDOW_FULLSCREEN_DESKTOP*/),
        destroy_window);

    if (window.get() == nullptr)
    {
        std::string err_message = SDL_GetError();
        clog << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        throw std::runtime_error(err_message);
    }

    int w;
    int h;
    SDL_GL_GetDrawableSize(window.get(), &w, &h);
    std::cout << "windows drawable_size: " << w << 'x' << h << std::endl;

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
    void pull_system_events(bool& continue_loop);

    static constexpr size_t fbo_width  = 1024;
    static constexpr size_t fbo_height = 1024;

    properties_reader properties;

    std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window;
    std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, decltype(&SDL_GL_DestroyContext)> context;

    gles30::shader depth_shader;
    gles30::shader shader_shadow;

    gles30::mesh mesh_floor;
    gles30::mesh mesh_cube;

    gles30::texture depth_texture;
    gles30::mesh    mesh_quad;

    gles30::framebuffer depth_fbo;

    gles30::texture wood_texture;

    bool use_perspective_matrix = false;
};

void scene::pull_system_events(bool& continue_loop)
{
    using namespace std;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (SDL_EVENT_FINGER_DOWN == event.type || SDL_EVENT_QUIT == event.type)
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
                use_perspective_matrix = !use_perspective_matrix;
            }
            else if (event.key.key == SDLK_1)
            {
                // https://stackoverflow.com/questions/27024884/srgb-framebuffer-on-opengl-es-3-0
                // OpenGL ES 3.0 - by default use RGB back_buffer and to change
                // it you have to attach sRGB texture. On Desktop we
                // can play with it in runtime
                if (gles30::is_desktop())
                {
                    static bool is_enabled            = false;
                    is_enabled                        = !is_enabled;
                    constexpr int GL_FRAMEBUFFER_SRGB = 0x8DB9;
                    if (is_enabled)
                    {
                        glEnable(GL_FRAMEBUFFER_SRGB);
                    }
                    else
                    {
                        glDisable(GL_FRAMEBUFFER_SRGB);
                    }
                    std::cout
                        << "GL_FRAMEBUFFER_SRGB enabled: " << std::boolalpha
                        << is_enabled << std::endl;
                }
            }
            else if (event.key.key == SDLK_2)
            {
            }
            else if (event.key.key == SDLK_3)
            {
            }
            else if (event.key.key == SDLK_4)
            {
            }
            else if (event.key.key == SDLK_5)
            {
                if (!SDL_SetWindowRelativeMouseMode(SDL_GetKeyboardFocus(), true))
                {
                    throw std::runtime_error(SDL_GetError());
                }
            }
            else if (event.key.key == SDLK_6)
            {
                if (!SDL_SetWindowRelativeMouseMode(SDL_GetKeyboardFocus(), false))
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
            gles30::print_view_port();
        }
    }
}

scene::scene()
    : properties("res/runtime.properties.hxx")
    , window{ create_window(properties, gles30::multisampling::disable) }
    , context{ create_opengl_context(window.get()) }
    , depth_shader{ "res/depth.vsh", "res/depth.fsh" }
    , shader_shadow{ "res/shadow.vsh", "res/shadow.fsh" }
    , mesh_floor{ create_mesh(
          plane_vertices.data(), plane_vertices.size() / 8, { &wood_texture }) }
    , mesh_cube{ create_mesh(
          cube_vertices.data(), cube_vertices.size() / 8, { &wood_texture }) }
    , depth_texture{ gles30::texture::type::depth_component,
                     fbo_width,
                     fbo_height,
                     gles30::texture::pixel_type::gl_float }
    , mesh_quad{ create_mesh(
          quad_virtices.data(), quad_virtices.size() / 8, { &depth_texture }) }
    , depth_fbo{ fbo_width, fbo_height, gles30::generate_render_object::no }
    , wood_texture("res/wood.png", gles30::texture::type::diffuse)
{
    depth_fbo.depth_attachment(depth_texture);
    if (!depth_fbo.is_complete())
    {
        throw std::runtime_error("depth_fbo not complete");
    }
    depth_fbo.unbind();

    create_camera(properties);
}

void scene::render([[maybe_unused]] float delta_time)
{
    camera.move_using_keyboard_wasd(delta_time);

    glEnable(GL_DEPTH_TEST);

    /// 1. render depth to texture
    depth_texture.bind();
    depth_fbo.bind();

    clear_back_buffer(properties.get_vec3("clear_color"));

    float     near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 light_projection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    light_pos     = properties.get_vec3("light_pos");
    light_look_at = properties.get_vec3("light_look_at");

    glm::mat4 light_view =
        glm::lookAt(light_pos, light_look_at, glm::vec3(0.0f, 1.0f, 0.0f));

    depth_shader.use();
    depth_shader.set_uniform("model", glm::mat4(1.f));
    if (use_perspective_matrix)
    {
        // shadow not working for camera.projection_matrix() - why?
        depth_shader.set_uniform("view", light_view);
        depth_shader.set_uniform("projection", camera.projection_matrix());
    }
    else
    {
        depth_shader.set_uniform("view", light_view);
        depth_shader.set_uniform("projection", light_projection);
    }

    glViewport(0, 0, fbo_width, fbo_height);

    mesh_floor.textures_disable();
    mesh_cube.textures_disable();

    mesh_floor.draw(depth_shader);
    mesh_cube.draw(depth_shader);

    depth_fbo.unbind();

    /// 2. render texture with debug quad

    glViewport(0, 0, screen_width, screen_height);
    clear_back_buffer(properties.get_vec3("clear_color"));

    shader_shadow.use();

    shader_shadow.set_uniform("view", camera.view_matrix());
    shader_shadow.set_uniform("projection", camera.projection_matrix());
    shader_shadow.set_uniform("model", glm::mat4(1.f));
    shader_shadow.set_uniform("light_space_matrix",
                              light_projection * light_view);
    shader_shadow.set_uniform("light_pos", light_pos);
    shader_shadow.set_uniform("view_pos", camera.position());

    // we need set by hand third texture - shadow_map - see res/shadow.fsh
    shader_shadow.set_uniform("tex_shadow_map", depth_texture, 2);

    mesh_floor.textures_enable();
    mesh_cube.textures_enable();

    mesh_cube.draw(shader_shadow);
    mesh_floor.draw(shader_shadow);
}

int main(int /*argc*/, char* /*argv*/[])
{
    gles30::windows_make_process_dpi_aware();

    {
        scene scene;

        float last_frame_time = 0.0f; // Time of last frame

        for (bool continue_loop = true; continue_loop;
             SDL_GL_SwapWindow(scene.window.get()))
        {
            float delta_time = update_delta_time(last_frame_time);

            scene.properties.update_changes();

            scene.pull_system_events(continue_loop);

            scene.render(delta_time);
        }
    }

    return 0;
}

// clang-format off
const std::array<float, 6 * 8> quad_virtices = {{
 // positions         // normals          // texture coords
-1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
 1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 0.0f,

-1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
 1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
 1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 1.0f
}};

const std::array<float, 36 * 8> cube_vertices = {{
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
}};

const std::array<float, 6 * 8> plane_vertices = {{
    // positions            // normals         // texcoords
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
}};

// clang-format on
