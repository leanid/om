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

#pragma pack(push, 4)
struct context_parameters
{
    const char* name          = nullptr;
    int32_t     major_version = 0;
    int32_t     minor_version = 0;
    int32_t     profile_type  = 0;
};
#pragma pack(pop)

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

extern const float     cube_vertices[36 * 8];
extern const glm::vec3 pointLightPositions[4];

void set_cube_vertex_attributes()
{
    // now tell OpenGL how to interpret data from VBO
    GLuint    location_of_vertex_attribute = 0; // position
    int       size_of_attribute            = 3; // 3 float values (x, y, z)
    GLenum    type_of_data   = GL_FLOAT; // all values in vec{2,3,4} of float
    GLboolean normalize_data = GL_FALSE; // OpenGL can normalize values
    // to [0, 1] - for unsigned and to [-1, 1] for signed values
    int stride =
        (3 + 3 + 2) * sizeof(float); // step in bytes from one attribute to next
    void* start_of_data_offset = nullptr; // we start from begin of buffer
    glVertexAttribPointer(location_of_vertex_attribute, size_of_attribute,
                          type_of_data, normalize_data, stride,
                          start_of_data_offset);
    gl_check();

    glEnableVertexAttribArray(0);
    gl_check();

    location_of_vertex_attribute = 1; // normal
    size_of_attribute            = 3; // x, y, z
    type_of_data                 = GL_FLOAT;
    normalize_data               = GL_FALSE;
    start_of_data_offset         = reinterpret_cast<void*>(3 * sizeof(float));
    glVertexAttribPointer(location_of_vertex_attribute, size_of_attribute,
                          type_of_data, normalize_data, stride,
                          start_of_data_offset);
    gl_check();

    glEnableVertexAttribArray(1);
    gl_check();

    location_of_vertex_attribute = 2; // texCoords
    size_of_attribute            = 2; // x, y
    type_of_data                 = GL_FLOAT;
    normalize_data               = GL_FALSE;
    start_of_data_offset         = reinterpret_cast<void*>(6 * sizeof(float));
    glVertexAttribPointer(location_of_vertex_attribute, size_of_attribute,
                          type_of_data, normalize_data, stride,
                          start_of_data_offset);
    gl_check();
    glEnableVertexAttribArray(2);
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    using namespace std::chrono;

    properties_reader properties("./res/runtime.properties.hxx");

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        return -1;
    }

    title = properties.get_string("title");

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 640, 480,
                         ::SDL_WINDOW_OPENGL | ::SDL_WINDOW_RESIZABLE),
        SDL_DestroyWindow);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return -1;
    }

    int                r;
    context_parameters ask_context;

    using namespace std::string_literals;

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

    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            ask_context.profile_type);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                            ask_context.major_version);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                            ask_context.minor_version);
    SDL_assert_always(r == 0);

    unique_ptr<void, void (*)(void*)> gl_context(
        SDL_GL_CreateContext(window.get()), SDL_GL_DeleteContext);
    if (nullptr == gl_context)
    {
        clog << "Failed to create: " << ask_context
             << " error: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    context_parameters got_context = ask_context;

    int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                                     &got_context.major_version);
    assert(result == 0);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                 &got_context.minor_version);
    assert(result == 0);

    clog << "Ask for " << ask_context << endl;
    clog << "Receive " << got_context << endl;
    clog << "default ";
    print_view_port();

    namespace fs = std::filesystem;
    using namespace gles30;

    shader nanosuit_shader(fs::path{ "./res/vertex_pos.vsh" },
                           "./res/material.fsh");
    shader light_shader(fs::path{ "./res/vertex_pos.vsh" },
                        "./res/lamp_color.fsh");

    // generate OpenGL object id for future VertexBufferObject
    uint32_t cube_vbo;
    glGenBuffers(1, &cube_vbo);
    gl_check();

    // GL_ARRAY_BUFFER - is VertexBufferObject type
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    gl_check();

    // copy vertex data into GPU memory
    // Using newly created VBO
    //
    // GL_STATIC_DRAW: the data will most likely not change at all or
    //    very rarely.
    // GL_DYNAMIC_DRAW: the data is likely to change a lot.
    // GL_STREAM_DRAW: the data will change every time it is drawn.
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
                 GL_STATIC_DRAW);
    gl_check();

    uint32_t EBO; // ElementBufferObject - indices buffer
    glGenBuffers(1, &EBO);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

    uint32_t cube_indexes[36];
    std::iota(begin(cube_indexes), end(cube_indexes), 0);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indexes), cube_indexes,
                 GL_STATIC_DRAW);
    gl_check();

    // Generate VAO VertexArrayState object to remember current VBO and
    // EBO(if any) with all attributes parameters stored in one object
    // called VAO think it is current VBO + EBO + attributes state in one
    // object
    uint32_t cube_vao;
    glGenVertexArrays(1, &cube_vao);
    gl_check();
    glBindVertexArray(cube_vao);
    gl_check();

    set_cube_vertex_attributes();

    [[maybe_unused]] GLenum primitive_render_mode = GL_TRIANGLES;

    float lastFrame = 0.0f; // Time of last frame

    camera = fps_camera(/*pos*/ { 0, 0, 1 }, /*dir*/ { 0, 0, -1 },
                        /*up*/ { 0, 1, 0 });

    fovy = properties.get_float("fovy");
    camera.fovy(fovy);
    aspect = properties.get_float("aspect");
    camera.aspect(aspect);

    glEnable(GL_DEPTH_TEST);
    gl_check();

    gles30::model nanosuit("./res/model/nanosuit.obj");

    bool continue_loop = true;
    while (continue_loop)
    {
        float currentFrame = SDL_GetTicks() * 0.001f; // seconds
        float deltaTime    = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        properties.update_changes();

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
                        glViewport(0, 0, event.window.data1,
                                   event.window.data2);
                        gl_check();
                        print_view_port();
                        break;
                }
            }
        }

        camera.move_using_keyboard_wasd(deltaTime);

        glm::mat4 model{ 1 };
        glm::mat4 view       = camera.view_matrix();
        glm::mat4 projection = camera.projection_matrix();

        clear_color = properties.get_vec3("clear_color");
        float red   = clear_color.r;
        float green = clear_color.g;
        float blue  = clear_color.b;
        float alpha = 0.f;

        glClearColor(red, green, blue, alpha);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render nanosuit model
        {
            // glBindVertexArray(nanosuit_VAO);
            // gl_check();

            // enable new shader program
            nanosuit_shader.use();

            light_ambient  = properties.get_vec3("light_ambient");
            light_diffuse  = properties.get_vec3("light_diffuse");
            light_specular = properties.get_vec3("light_specular");
            light_pos      = camera.position();

            material_shininess = properties.get_float("material_shininess");
            material_specular  = properties.get_vec3("material_specular");

            nanosuit_shader.set_uniform("material.shininess",
                                        material_shininess);
            // material.set_uniform("material.diffuse", diffuse_map, 0);
            // material.set_uniform("material.specular", specular_map, 1);

            glm::mat4 rotated_model{ model };
            angle += properties.get_float("angle");
            rotate_axis = properties.get_vec3("rotate_axis");

            std::vector<std::string> names{
                "pointLights[0].position",  "pointLights[0].ambient",
                "pointLights[0].diffuse",   "pointLights[0].specular",
                "pointLights[0].constant",  "pointLights[0].linear",
                "pointLights[0].quadratic",
            };

            // directional light
            nanosuit_shader.set_uniform("dirLight.direction",
                                        { -0.2f, -1.0f, -0.3f });
            nanosuit_shader.set_uniform("dirLight.ambient",
                                        { 0.05f, 0.05f, 0.05f });
            nanosuit_shader.set_uniform("dirLight.diffuse",
                                        { 0.4f, 0.4f, 0.4f });
            nanosuit_shader.set_uniform("dirLight.specular",
                                        { 0.5f, 0.5f, 0.5f });
            // point lights
            std::array<size_t, std::size(pointLightPositions)> indexes{};
            std::iota(begin(indexes), end(indexes), 0);
            std::for_each(
                begin(indexes), end(indexes),
                [&nanosuit_shader, &names](size_t index) {
                    char   i        = static_cast<char>('0' + index);
                    size_t zero_pos = names.front().find('[') + 1;

                    std::for_each(
                        begin(names), end(names),
                        [i, zero_pos](std::string& v) { v[zero_pos] = i; });

                    nanosuit_shader.set_uniform(names[0],
                                                pointLightPositions[index]);
                    nanosuit_shader.set_uniform(names[1],
                                                { 0.05f, 0.05f, 0.05f });
                    nanosuit_shader.set_uniform(names[2], { 0.8f, 0.8f, 0.8f });
                    nanosuit_shader.set_uniform(names[3], { 1.0f, 1.0f, 1.0f });
                    nanosuit_shader.set_uniform(names[4], 1.0f);
                    nanosuit_shader.set_uniform(names[5], 0.09f);
                    nanosuit_shader.set_uniform(names[6], 0.032f);
                });

            // spot light
            nanosuit_shader.set_uniform("spot_light.position",
                                        camera.position());
            nanosuit_shader.set_uniform("spot_light.direction",
                                        camera.direction());
            nanosuit_shader.set_uniform("spot_light.ambient",
                                        { 0.0f, 0.0f, 0.0f });
            nanosuit_shader.set_uniform("spot_light.diffuse",
                                        { 1.0f, 1.0f, 1.0f });
            nanosuit_shader.set_uniform("spot_light.specular",
                                        { 1.0f, 1.0f, 1.0f });
            nanosuit_shader.set_uniform("spot_light.constant", 1.0f);
            nanosuit_shader.set_uniform("spot_light.linear", 0.09f);
            nanosuit_shader.set_uniform("spot_light.quadratic", 0.032f);
            nanosuit_shader.set_uniform("spot_light.cut_off",
                                        glm::cos(glm::radians(12.5f)));
            nanosuit_shader.set_uniform("spot_light.outer_cut_off",
                                        glm::cos(glm::radians(15.0f)));

            rotated_model = glm::rotate(rotated_model, angle, rotate_axis);

            nanosuit_shader.set_uniform("model", rotated_model);
            nanosuit_shader.set_uniform("view", view);
            nanosuit_shader.set_uniform("projection", projection);

            nanosuit.draw(nanosuit_shader);
        }
        {
            // also draw the lamp object(s)
            light_shader.use();
            light_shader.set_uniform("projection", projection);
            light_shader.set_uniform("view", view);

            // we now draw as many light bulbs as we have point lights.
            glBindVertexArray(cube_vao);
            for (auto pointLightPosition : pointLightPositions)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPosition);
                model = glm::scale(model,
                                   glm::vec3(0.2f)); // Make it a smaller cube
                light_shader.set_uniform("model", model);
                glDrawArrays(primitive_render_mode, 0, 36);
            }
        }

        SDL_GL_SwapWindow(window.get());
    }

    SDL_Quit();

    return 0;
}

// clang-format off
const glm::vec3 pointLightPositions[4] = { glm::vec3(0.7f, 0.2f, 2.0f),
                                           glm::vec3(2.3f, -3.3f, -4.0f),
                                           glm::vec3(-4.0f, 2.0f, -12.0f),
                                           glm::vec3(0.0f, 0.0f, -3.0f) };


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
