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

extern float quadVertices[6 * 8];

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

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

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
    void create_uniform_buffer(const void*            buffer_ptr,
                               const size_t           buffer_size,
                               const std::string_view block_name,
                               const uint32_t         binding_point,
                               gles30::shader&        shader,
                               uint32_t&              ubo_handle);
    void pull_system_events(bool& continue_loop, int& current_effect);
    void regenerate_rock_matrixes();

    properties_reader properties;

    std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window;
    std::unique_ptr<void, void (*)(void*)>             context;

    gles30::shader instanced_shader;
    gles30::shader planet_shader;
    gles30::mesh   quad;
    uint32_t       instance_vbo;
    size_t         num_instances = 1000;

    gles30::model          planet_mars;
    gles30::model          rock;
    std::vector<glm::mat4> rock_matrices;
    bool                   use_instance_draw = false;
};

void scene::create_uniform_buffer(const void*            buffer_ptr,
                                  const size_t           buffer_size,
                                  const std::string_view block_name,
                                  const uint32_t         binding_point,
                                  gles30::shader&        shader,
                                  uint32_t&              ubo_handle)
{
    glGenBuffers(1, &ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER,
                 buffer_size,
                 buffer_ptr,
                 GL_STATIC_DRAW); // allocate "buffer_size" bytes of memory

    shader.bind_uniform_block(block_name, binding_point);

    // bind UBO buffer to Binding Point "binding_point"
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo_handle);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void scene::pull_system_events(bool& continue_loop, int& current_effect)
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
                use_instance_draw = !use_instance_draw;
            }
            else if (event.key.key == SDLK_1)
            {
                if (use_instance_draw)
                {
                    num_instances *= 2;
                    regenerate_rock_matrixes();
                }
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

void scene::regenerate_rock_matrixes()
{
    unsigned int amount = num_instances;
    rock_matrices.resize(amount);

    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range
        // [-offset, offset]
        float angle_rock = (float)i / (float)amount * 360.0f;
        float displacement =
            (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x      = sin(angle_rock) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y =
            displacement *
            0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z      = cos(angle_rock) * radius + displacement;
        model        = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model       = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked
        // rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        rock_matrices[i] = model;
    }

    // create OpenGL buffer and load with data
    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::mat4) * num_instances,
                 &rock_matrices[0],
                 GL_STATIC_DRAW);

    std::cout << "num_instances = " << num_instances << std::endl;
}

scene::scene()
    : properties("res/runtime.properties.hxx")
    , window{ create_window(properties) }
    , context{ create_opengl_context(window.get()) }
    , instanced_shader("res/instanced.vsh", "res/instanced.fsh")
    , planet_shader("res/textured.vsh", "res/textured.fsh")
    , quad{ create_mesh(quadVertices, sizeof(quadVertices) / 4 / 8, {}) }
    , instance_vbo{}
    , planet_mars("res/planet.obj")
    , rock("res/rock.obj")
{
    glGenBuffers(1, &instance_vbo);
    create_camera(properties);

    // generate offset positions
    regenerate_rock_matrixes();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void scene::render([[maybe_unused]] float delta_time)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    clear_back_buffer(properties.get_vec3("clear_color"));

    instanced_shader.use();

    std::string validation_result = instanced_shader.validate();
    if (!validation_result.empty())
    {
        std::cout << validation_result << std::endl;
    }

    planet_shader.use();

    camera.move_using_keyboard_wasd(delta_time);

    planet_shader.set_uniform("projection", camera.projection_matrix());
    planet_shader.set_uniform("view", camera.view_matrix());
    planet_shader.set_uniform("model", glm::mat4(1.0f));

    planet_mars.draw(planet_shader);

    if (use_instance_draw)
    {
        instanced_shader.use();
        instanced_shader.set_uniform("projection", camera.projection_matrix());
        instanced_shader.set_uniform("view", camera.view_matrix());

        rock.draw_instanced(
            instanced_shader,
            num_instances,
            [&]
            {
                //        // explain data for OpenGL
                glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);

                // vertex attributes
                std::size_t vec4Size = sizeof(glm::vec4);
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(
                    3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4,
                                      4,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      4 * vec4Size,
                                      (void*)(1 * vec4Size));
                glEnableVertexAttribArray(5);
                glVertexAttribPointer(5,
                                      4,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      4 * vec4Size,
                                      (void*)(2 * vec4Size));
                glEnableVertexAttribArray(6);
                glVertexAttribPointer(6,
                                      4,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      4 * vec4Size,
                                      (void*)(3 * vec4Size));

                glVertexAttribDivisor(3, 1);
                glVertexAttribDivisor(4, 1);
                glVertexAttribDivisor(5, 1);
                glVertexAttribDivisor(6, 1);
            });
    }
    else
    {
        for (size_t i = 0; i < num_instances; ++i)
        {
            planet_shader.set_uniform("model", rock_matrices[i]);
            rock.draw(planet_shader);
        }
    }
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

            scene.pull_system_events(continue_loop, current_post_process);

            scene.render(delta_time);

            SDL_GL_SwapWindow(scene.window.get());
        }
    }

    return 0;
}

// clang-format off
float quadVertices[6*8] = {
    // positions           //normals         // uv
    -0.01f,  0.01f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.01f, -0.01f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.01f, -0.01f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -0.01f,  0.01f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.01f, -0.01f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.01f,  0.01f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};
// clang-format on
