#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

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
    const char* name          = nullptr;
    int32_t     major_version = 0;
    int32_t     minor_version = 0;
    int32_t     profile_type  = 0;
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
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                            ask_context.major_version);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                            ask_context.minor_version);
    SDL_assert_always(r == 0);

    unique_ptr<void, void (*)(void*)> gl_context(SDL_GL_CreateContext(window),
                                                 SDL_GL_DestroyContext);
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
    assert(result == 0);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                 &got_context.minor_version);
    assert(result == 0);
    int got_depth_size = 0;
    result = SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &got_depth_size);
    assert(result == 0);
    int stensil_size = 0;
    result           = SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stensil_size);
    assert(result == 0);

    clog << "current context have DEPTH_SIZE: " << got_depth_size << std::endl;
    clog << "current context have STENSIL_SIZE: " << stensil_size << std::endl;

    if (ask_context != got_context)
    {
        clog << "Ask for " << ask_context << endl;
        clog << "Receive " << got_context << endl;
    }

    initialize_opengles_3_2();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

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
        else if (SDL_MOUSEMOTION == event.type)
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
            if (event.key.keysym.sym == SDLK_0)
            {
                current_effect = 5;
            }
            else if (event.key.keysym.sym == SDLK_1)
            {
                current_effect = 1;
            }
            else if (event.key.keysym.sym == SDLK_2)
            {
                current_effect = 2;
            }
            else if (event.key.keysym.sym == SDLK_3)
            {
                current_effect = 3;
            }
            else if (event.key.keysym.sym == SDLK_4)
            {
                current_effect = 4;
            }
            else if (event.key.keysym.sym == SDLK_5)
            {
                if (0 != SDL_SetRelativeMouseMode(SDL_TRUE))
                {
                    throw std::runtime_error(SDL_GetError());
                }
            }
            else if (event.key.keysym.sym == SDLK_6)
            {
                if (0 != SDL_SetRelativeMouseMode(SDL_FALSE))
                {
                    throw std::runtime_error(SDL_GetError());
                }
            }
        }
        else if (SDL_WINDOWEVENT == event.type)
        {
            switch (event.window.event)
            {
                case ::SDL_WindowEventID::SDL_WINDOWEVENT_RESIZED:
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
                    break;
            }
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

std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> create_window(
    const properties_reader& properties)
{
    using namespace std;
    const int init_result = SDL_Init(SDL_INIT_VIDEO);
    if (init_result != 0)
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

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), static_cast<int>(screen_width), static_cast<int>(screen_height), ::SDL_WINDOW_OPENGL | ::SDL_WINDOW_RESIZABLE),
        SDL_DestroyWindow);

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
    gles30::shader outline;
    gles30::shader transparend_shader;
    gles30::shader quad_shader;
    gles30::shader skybox_shader;

    gles30::texture tex_marble;
    gles30::texture tex_metal;
    gles30::texture tex_grass;
    gles30::texture tex_window;
    gles30::texture tex_color_buffer;
    gles30::texture tex_cubemap;

    gles30::mesh cube_marble;
    gles30::mesh cube_metal;
    gles30::mesh cube_skybox;
    gles30::mesh plane_metal;
    gles30::mesh transparent_quad;
    gles30::mesh fullscreen_quad;

    std::vector<glm::vec3> vegetation;

    gles30::framebuffer frame;
};

scene::scene()
    : properties("res/runtime.properties.hxx")
    , window{ create_window(properties) }
    , context{ create_opengl_context(window.get()) }
    , cube_shader("res/cube.vsh", "res/cube.fsh")
    , outline("res/cube.vsh", "res/outline.fsh")
    , transparend_shader("res/cube.vsh", "res/discard.fsh")
    , quad_shader("res/quad_vertex.vsh", "res/quad_frag.fsh")
    , skybox_shader("res/skybox.vsh", "res/skybox.fsh")
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
    , cube_marble{ create_mesh(
          cube_vertices, sizeof(cube_vertices) / 4 / 8, { &tex_marble }) }
    , cube_metal{ create_mesh(
          cube_vertices, sizeof(cube_vertices) / 4 / 8, { &tex_metal }) }
    , cube_skybox{ create_mesh(
          cube_vertices, sizeof(cube_vertices) / 4 / 8, { &tex_cubemap }) }
    , plane_metal{ create_mesh(
          plane_vertices, sizeof(plane_vertices) / 4 / 8, { &tex_metal }) }
    , transparent_quad{ create_mesh(
          transparent_vert, sizeof(transparent_vert) / 4 / 8, { &tex_window }) }
    , fullscreen_quad{ create_mesh(fullscreen_vertices,
                                   sizeof(fullscreen_vertices) / 4 / 8,
                                   { &tex_color_buffer }) }
    , vegetation{ glm::vec3(-1.5f, 0.0f, -0.48f),
                  glm::vec3(1.5f, 0.0f, 0.51f),
                  glm::vec3(0.0f, 0.0f, 0.7f),
                  glm::vec3(-0.3f, 0.0f, -2.3f),
                  glm::vec3(0.5f, 0.0f, -0.6f) }
    , frame(properties.get_float("screen_width"),
            properties.get_float("screen_height"))
{
    create_camera(properties);

    frame.color_attachment(tex_color_buffer);

    if (!frame.is_complete())
    {
        throw std::runtime_error("framebuffer incomplete");
    }
}

void scene::render(float delta_time)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera.move_using_keyboard_wasd(delta_time);

    clear_back_buffer(properties.get_vec3("clear_color"));

    float scale = 1.0f;

    render_mesh(cube_shader,
                camera,
                plane_metal,
                glm::vec3(0.0f, 0.0f, 0.0f),
                scale,
                properties,
                render_options::only_pro_view_model);

    render_mesh(cube_shader,
                camera,
                cube_marble,
                glm::vec3(-1.0f, 0.0f, -1.0f),
                scale,
                properties,
                render_options::only_pro_view_model);
    render_mesh(cube_shader,
                camera,
                cube_metal,
                glm::vec3(2.0f, 0.0f, 0.0f),
                scale,
                properties,
                render_options::only_pro_view_model);

    transparend_shader.use();
    sort_transparent_quads = properties.get_bool("sort_transparent_quads");
    if (sort_transparent_quads)
    {
        glm::vec3 cam_position = camera.position();
        // we want to sort in order of far from camera
        std::sort(begin(vegetation),
                  end(vegetation),
                  [&cam_position](const glm::vec3& l, const glm::vec3& r) {
                      return glm::length(l - cam_position) >
                             glm::length(r - cam_position);
                  });
    }

    for (auto pos : vegetation)
    {
        render_mesh(transparend_shader,
                    camera,
                    transparent_quad,
                    pos,
                    scale,
                    properties,
                    render_options::only_pro_view_model);
    }

    glDepthFunc(GL_LEQUAL);
    render_mesh(skybox_shader,
                camera,
                cube_skybox,
                camera.position(),
                1.f,
                properties,
                render_options::only_pro_view);
    glDepthFunc(GL_LESS);
}

void scene::render_fullscreen_quad()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    render_mesh(quad_shader,
                camera,
                fullscreen_quad,
                glm::vec3(0, 0, 0),
                1.0f,
                properties,
                render_options::no_matrix);
}

int main(int /*argc*/, char* /*argv*/[])
{
    scene scene;

    float last_frame_time      = 0.0f; // Time of last frame
    int   current_post_process = 0;

    for (bool continue_loop = true; continue_loop;)
    {
        float delta_time = update_delta_time(last_frame_time);

        scene.properties.update_changes();

        pull_system_events(continue_loop, current_post_process);

        scene.frame.bind();
        scene.render(delta_time);

        scene.frame.unbind();
        scene.quad_shader.use();
        scene.quad_shader.set_uniform("current_post_process",
                                      current_post_process);
        scene.render_fullscreen_quad();

        SDL_GL_SwapWindow(scene.window.get());
    }

    SDL_Quit();

    return 0;
}

// clang-format off
const float cube_vertices[36 * 8] = {
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

const float plane_vertices[6 * 8] = {
    // positions                            // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
     5.0f, -0.5f,  5.0f, 0.0f, 0.0f, 0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f, 0.0f, 0.0f, 0.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f, 0.0f, 0.0f, 0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f, 0.0f, 0.0f, 0.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f, 0.0f, 0.0f, 0.0f,  2.0f, 2.0f
};

const float transparent_vert[6 * 8] = {
        // positions        // normal          // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f
    };
const float fullscreen_vertices[6 * 8]{
        // positions       // normal       // texCoords
        -1.0f,  1.0f, 0.f, 0.f, 0.f, 0.f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.f, 0.f, 0.f, 0.f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.f, 0.f, 0.f, 0.f,  1.0f, 1.0f
};
// clang-format on
