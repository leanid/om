#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "fps_camera.hxx"
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

int get_z_buf_operation(std::string_view name)
{
    auto it = std::find_if(begin(z_buf_operations), end(z_buf_operations),
                           [&name](const std::pair<std::string_view, int>& p) {
                               return p.first == name;
                           });
    if (it == end(z_buf_operations))
    {
        throw std::out_of_range(std::string("z_buf_operation not found: ") +
                                std::string(name));
    }
    return it->second;
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
    gl_check();
    clog << "view port is: x=" << view_port[0] << " y=" << view_port[1]
         << " w=" << view_port[2] << " h=" << view_port[3] << endl;
}

extern const float cube_vertices[36 * 8];

void render_light_cubes(gles30::shader& cube_shader, const fps_camera& camera,
                        const gles30::mesh& mesh, glm::vec3 position)
{
    // also draw the lamp object(s)
    cube_shader.use();
    cube_shader.set_uniform("projection", camera.projection_matrix());
    cube_shader.set_uniform("view", camera.view_matrix());

    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, position);
        model           = glm::scale(model, glm::vec3(1.f));
        cube_shader.set_uniform("model", model);
        mesh.draw(cube_shader);
    }
}

[[nodiscard]] std::unique_ptr<void, void (*)(void*)> create_opengl_context(
    SDL_Window* window)
{
    using namespace std;
    context_parameters ask_context;

    string_view platform_name = SDL_GetPlatform();

    const array<string_view, 3> desktop_platforms{ "Windows", "Mac OS X",
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
        // we want OpenGL ES 3.0 context
        ask_context.name          = "OpenGL ES";
        ask_context.major_version = 3;
        ask_context.minor_version = 0;
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
                                                 SDL_GL_DeleteContext);
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

    if (ask_context != got_context)
    {
        clog << "Ask for " << ask_context << endl;
        clog << "Receive " << got_context << endl;
    }

    glEnable(GL_DEPTH_TEST);
    gl_check();

    return gl_context;
};

void pull_system_events(bool& continue_loop, GLenum& primitive_render_mode)
{
    using namespace std;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (SDL_FINGERDOWN == event.type)
        {
            continue_loop = false;
            break;
        }
        else if (SDL_QUIT == event.type)
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
        else if (SDL_MOUSEWHEEL == event.type)
        {
            camera.zoom(-event.wheel.y);
        }
        else if (SDL_KEYUP == event.type)
        {
            // OpenGL ES 3.0 did't have glPolygonMode
            // so we try to emulate it with next render primitive types
            if (event.key.keysym.sym == SDLK_1)
            {
                primitive_render_mode = GL_TRIANGLES;
            }
            else if (event.key.keysym.sym == SDLK_2)
            {
                primitive_render_mode = GL_LINES;
            }
            else if (event.key.keysym.sym == SDLK_3)
            {
                primitive_render_mode = GL_LINE_STRIP;
            }
            else if (event.key.keysym.sym == SDLK_4)
            {
                primitive_render_mode = GL_LINE_LOOP;
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
                    gl_check();
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
    gl_check();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_check();
}

std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> create_window(
    const properties_reader& properties)
{
    using namespace std;
    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0)
    {
        std::string err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        throw std::runtime_error(err_message);
    }

    title         = properties.get_string("title");
    screen_width  = properties.get_float("screen_width");
    screen_height = properties.get_float("screen_height");

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, screen_width, screen_height,
                         ::SDL_WINDOW_OPENGL | ::SDL_WINDOW_RESIZABLE),
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

gles30::mesh create_cube_mesh(gles30::texture* texture)
{
    using namespace std;
    vector<gles30::vertex> cube_vert;
    cube_vert.reserve(sizeof(cube_vertices) / 4 / 8);
    for (size_t i = 0; i < sizeof(cube_vertices) / 4; i += 8)
    {
        gles30::vertex v;
        v.position.x = cube_vertices[i + 0];
        v.position.y = cube_vertices[i + 1];
        v.position.z = cube_vertices[i + 2];

        v.normal.x = cube_vertices[i + 3];
        v.normal.y = cube_vertices[i + 4];
        v.normal.z = cube_vertices[i + 5];

        v.uv.x = cube_vertices[i + 6];
        v.uv.y = cube_vertices[i + 7];

        cube_vert.push_back(v);
    }
    vector<uint32_t> cube_indexes(36);
    std::iota(begin(cube_indexes), end(cube_indexes), 0);

    return gles30::mesh(std::move(cube_vert), std::move(cube_indexes),
                        { texture });
}

void create_camera(const properties_reader& properties)
{
    cam_pos = properties.get_vec3("cam_pos");
    cam_dir = properties.get_vec3("cam_dir");

    camera = fps_camera(cam_pos, cam_dir,
                        /*up*/ { 0, 1, 0 });

    fovy = properties.get_float("fovy");
    camera.fovy(fovy);
    screen_aspect = properties.get_float("screen_aspect");
    camera.aspect(screen_aspect);
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    using namespace gles30;

    properties_reader properties("res/runtime.properties.hxx");

    auto window = create_window(properties);
    // destroy only on exit from main
    [[maybe_unused]] auto gl_context = create_opengl_context(window.get());

    shader light_cube_shader("res/cube.vsh", "res/cube.fsh");

    texture tex_marble("res/marble.jpg", texture::type::diffuse);
    texture tex_metal("res/metal.png", texture::type::diffuse);

    mesh cube_marble = create_cube_mesh(&tex_marble);
    mesh cube_metal  = create_cube_mesh(&tex_metal);

    [[maybe_unused]] GLenum primitive_render_mode = GL_TRIANGLES;

    float last_frame_time = 0.0f; // Time of last frame

    create_camera(properties);

    bool continue_loop = true;
    while (continue_loop)
    {
        float delta_time = update_delta_time(last_frame_time);

        properties.update_changes();

        int z_buf_op =
            get_z_buf_operation(properties.get_string("z_buf_operation"));
        glDepthFunc(z_buf_op);
        gl_check();

        pull_system_events(continue_loop, primitive_render_mode);

        camera.move_using_keyboard_wasd(delta_time);

        clear_back_buffer(properties.get_vec3("clear_color"));

        render_light_cubes(light_cube_shader, camera, cube_marble,
                           glm::vec3(-1.0f, 0.0f, -1.0f));
        render_light_cubes(light_cube_shader, camera, cube_metal,
                           glm::vec3(2.0f, 0.0f, 0.0f));

        SDL_GL_SwapWindow(window.get());
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
// clang-format on
