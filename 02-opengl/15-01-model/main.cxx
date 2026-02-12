#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <type_traits>

#include "fps_camera.hxx"
#include "gles30_model.hxx"
#include "gles30_shader.hxx"
#include "gles30_texture.hxx"
#include "opengles30.hxx"
#include "properties_reader.hxx"

#include "res/runtime.properties.hxx"

static fps_camera camera;

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

    std::array<GLint, 4> view_port{};
    glGetIntegerv(GL_VIEWPORT, view_port.data());
    gl_check();
    clog << "view port is: x=" << view_port[0] << " y=" << view_port[1]
         << " w=" << view_port[2] << " h=" << view_port[3] << endl;
}

extern const std::array<float, std::size_t{36} * std::size_t{8}> cube_vertices;
extern const glm::vec3 light_positions[4];

void render_light_cubes(gles30::shader&     light_cube_shader,
                        const fps_camera&   camera,
                        const gles30::mesh& mesh)
{
    // also draw the lamp object(s)
    light_cube_shader.use();
    light_cube_shader.set_uniform("projection", camera.projection_matrix());
    light_cube_shader.set_uniform("view", camera.view_matrix());

    // we now draw as many light bulbs as we have point lights.
    for (auto& light_position : light_positions)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, light_position);
        model           = glm::scale(model, glm::vec3(0.2f));
        light_cube_shader.set_uniform("model", model);
        mesh.draw(light_cube_shader);
    }
}

[[nodiscard]] std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, decltype(&SDL_GL_DestroyContext)> create_opengl_context(
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
        // we want OpenGL ES 3.0 context
        ask_context.name          = "OpenGL ES";
        ask_context.major_version = 3;
        ask_context.minor_version = 0;
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
    assert(result);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                 &got_context.minor_version);
    assert(result);

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
            // OpenGL ES 3.0 did't have glPolygonMode
            // so we try to emulate it with next render primitive types
            if (event.key.key == SDLK_1)
            {
                primitive_render_mode = GL_TRIANGLES;
            }
            else if (event.key.key == SDLK_2)
            {
                primitive_render_mode = GL_LINES;
            }
            else if (event.key.key == SDLK_3)
            {
                primitive_render_mode = GL_LINE_STRIP;
            }
            else if (event.key.key == SDLK_4)
            {
                primitive_render_mode = GL_LINE_LOOP;
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
            gl_check();
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
    gl_check();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_check();
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

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE),
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

gles30::mesh create_cube_mesh()
{
    using namespace std;
    vector<gles30::vertex> cube_vert;
    cube_vert.reserve(cube_vertices.size() / 8);
    for (size_t i = 0; i < cube_vertices.size(); i += 8)
    {
        gles30::vertex v;
        v.position.x = cube_vertices[i + 0];
        v.position.y = cube_vertices[i + 1];
        v.position.z = cube_vertices[i + 2];

        cube_vert.push_back(v);
    }
    vector<uint32_t> cube_indexes(36);
    std::iota(begin(cube_indexes), end(cube_indexes), 0);

    return gles30::mesh(std::move(cube_vert), std::move(cube_indexes), {});
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

// render nanosuit model
void render_nanosuit_model(gles30::shader&          nanosuit_shader,
                           const properties_reader& properties,
                           const gles30::model&     nanosuit,
                           const fps_camera&        camera)
{
    using namespace std;
    // enable new shader program
    nanosuit_shader.use();

    nanosuit_material_shininess =
        properties.get_float("nanosuit_material_shininess");
    nanosuit_material_ambient =
        properties.get_vec3("nanosuit_material_ambient");

    nanosuit_shader.set_uniform("material.ambient", nanosuit_material_ambient);
    nanosuit_shader.set_uniform("material.shininess",
                                nanosuit_material_shininess);

    // directional light
    dir_light_direction = properties.get_vec3("dir_light_direction");
    nanosuit_shader.set_uniform("direction_light.direction",
                                dir_light_direction);
    dir_light_ambient = properties.get_vec3("dir_light_ambient");
    nanosuit_shader.set_uniform("direction_light.ambient", dir_light_ambient);
    dir_light_diffuse = properties.get_vec3("dir_light_diffuse");
    nanosuit_shader.set_uniform("direction_light.diffuse", dir_light_diffuse);
    dir_light_specular = properties.get_vec3("dir_light_specular");
    nanosuit_shader.set_uniform("direction_light.specular", dir_light_specular);

    glm::mat4 model{ 1 };
    glm::mat4 rotated_model{ model };
    angle += properties.get_float("angle");
    rotate_axis = properties.get_vec3("rotate_axis");

    vector<string> names{
        "point_lights[0].position",  "point_lights[0].ambient",
        "point_lights[0].diffuse",   "point_lights[0].specular",
        "point_lights[0].constant",  "point_lights[0].linear",
        "point_lights[0].quadratic",
    };

    point_light_constant  = properties.get_float("point_light_constant");
    point_light_ambient   = properties.get_vec3("point_light_ambient");
    point_light_diffuse   = properties.get_vec3("point_light_diffuse");
    point_light_specular  = properties.get_vec3("point_light_specular");
    point_light_linear    = properties.get_float("point_light_linear");
    point_light_quadratic = properties.get_float("point_light_quadratic");

    // point lights
    for (auto& light_pos : light_positions)
    {
        auto   start_it = begin(light_positions);
        size_t index    = std::distance(start_it, &light_pos);
        char   i        = static_cast<char>('0' + index);
        size_t zero_pos = names.front().find('[') + 1;

        for (auto& name : names)
        {
            name[zero_pos] = i;
        }

        nanosuit_shader.set_uniform(names[0], light_pos);
        nanosuit_shader.set_uniform(names[1], point_light_ambient);
        nanosuit_shader.set_uniform(names[2], point_light_diffuse);
        nanosuit_shader.set_uniform(names[3], point_light_specular);
        nanosuit_shader.set_uniform(names[4], point_light_constant);
        nanosuit_shader.set_uniform(names[5], point_light_linear);
        nanosuit_shader.set_uniform(names[6], point_light_quadratic);
    };

    // spot light
    spot_light_ambient   = properties.get_vec3("spot_light_ambient");
    spot_light_diffuse   = properties.get_vec3("spot_light_diffuse");
    spot_light_specular  = properties.get_vec3("spot_light_specular");
    spot_light_constant  = properties.get_float("spot_light_constant");
    spot_light_linear    = properties.get_float("spot_light_linear");
    spot_light_quadratic = properties.get_float("spot_light_quadratic");
    spot_light_cut_off = properties.get_float("spot_light_cut_off"); // degrees
    spot_light_outer_cut_off =
        properties.get_float("spot_light_outer_cut_off"); // degrees

    nanosuit_shader.set_uniform("spot_light.position", camera.position());
    nanosuit_shader.set_uniform("spot_light.direction", camera.direction());
    nanosuit_shader.set_uniform("spot_light.ambient", spot_light_ambient);
    nanosuit_shader.set_uniform("spot_light.diffuse", spot_light_diffuse);
    nanosuit_shader.set_uniform("spot_light.specular", spot_light_specular);
    nanosuit_shader.set_uniform("spot_light.constant", spot_light_constant);
    nanosuit_shader.set_uniform("spot_light.linear", spot_light_linear);
    nanosuit_shader.set_uniform("spot_light.quadratic", spot_light_quadratic);
    nanosuit_shader.set_uniform("spot_light.cut_off",
                                cos(glm::radians(spot_light_cut_off)));
    nanosuit_shader.set_uniform("spot_light.outer_cut_off",
                                cos(glm::radians(spot_light_outer_cut_off)));

    rotated_model = glm::rotate(rotated_model, angle, rotate_axis);

    nanosuit_shader.set_uniform("model", rotated_model);

    glm::mat4 view       = camera.view_matrix();
    glm::mat4 projection = camera.projection_matrix();

    nanosuit_shader.set_uniform("view", view);
    nanosuit_shader.set_uniform("projection", projection);

    nanosuit.draw(nanosuit_shader);
};

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    using namespace gles30;

    properties_reader properties("res/runtime.properties.hxx");

    auto window = create_window(properties);
    // destroy only on exit from main
    [[maybe_unused]] auto gl_context = create_opengl_context(window.get());

    shader nanosuit_shader("res/nanosuit.vsh", "res/nanosuit.fsh");
    shader light_cube_shader("res/light_cube.vsh", "res/light_cube.fsh");

    mesh  cube_mesh = create_cube_mesh();
    model nanosuit("res/model/nanosuit.obj");

    [[maybe_unused]] GLenum primitive_render_mode = GL_TRIANGLES;

    float last_frame_time = 0.0f; // Time of last frame

    create_camera(properties);

    bool continue_loop = true;
    while (continue_loop)
    {
        float delta_time = update_delta_time(last_frame_time);

        properties.update_changes();

        pull_system_events(continue_loop, primitive_render_mode);

        camera.move_using_keyboard_wasd(delta_time);

        clear_back_buffer(properties.get_vec3("clear_color"));

        render_nanosuit_model(nanosuit_shader, properties, nanosuit, camera);

        render_light_cubes(light_cube_shader, camera, cube_mesh);

        SDL_GL_SwapWindow(window.get());
    }

    SDL_Quit();

    return 0;
}

// clang-format off
const glm::vec3 light_positions[4] = { glm::vec3(0.7f, 1.2f, 2.0f),
                                       glm::vec3(2.3f, 10.3f, -4.0f),
                                       glm::vec3(4.0f, 12.0f, 2.0f),
                                       glm::vec3(0.0f, 8.0f, -3.0f) };


const std::array<float, std::size_t{36} * std::size_t{8}> cube_vertices = {{
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
// clang-format on
