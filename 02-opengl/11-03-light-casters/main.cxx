#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <type_traits>

#include "fps_camera.hxx"
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

extern float vertices[36 * 8];

void update_vertex_attributes()
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
    glVertexAttribPointer(location_of_vertex_attribute,
                          size_of_attribute,
                          type_of_data,
                          normalize_data,
                          stride,
                          start_of_data_offset);
    gl_check();

    glEnableVertexAttribArray(0);
    gl_check();

    location_of_vertex_attribute = 1; // normal
    size_of_attribute            = 3; // x, y, z
    type_of_data                 = GL_FLOAT;
    normalize_data               = GL_FALSE;
    start_of_data_offset         = reinterpret_cast<void*>(3 * sizeof(float));
    glVertexAttribPointer(location_of_vertex_attribute,
                          size_of_attribute,
                          type_of_data,
                          normalize_data,
                          stride,
                          start_of_data_offset);
    gl_check();

    glEnableVertexAttribArray(1);
    gl_check();

    location_of_vertex_attribute = 2; // texCoords
    size_of_attribute            = 2; // x, y
    type_of_data                 = GL_FLOAT;
    normalize_data               = GL_FALSE;
    start_of_data_offset         = reinterpret_cast<void*>(6 * sizeof(float));
    glVertexAttribPointer(location_of_vertex_attribute,
                          size_of_attribute,
                          type_of_data,
                          normalize_data,
                          stride,
                          start_of_data_offset);
    gl_check();
    glEnableVertexAttribArray(2);
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    using namespace std::chrono;

    properties_reader properties("./res/runtime.properties.hxx");

    const bool init_result = SDL_Init(SDL_INIT_VIDEO);
    if (!init_result)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        return -1;
    }

    const std::string title = properties.get_string("title");

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow(title.c_str(), 640, 480, ::SDL_WINDOW_OPENGL | ::SDL_WINDOW_RESIZABLE),
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

    const char* platform_name = SDL_GetPlatform();
    if (platform_name == "Windows"s || platform_name == "Mac OS X"s ||
        platform_name == "Linux"s)
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
    SDL_assert_always(r);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                            ask_context.major_version);
    SDL_assert_always(r);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                            ask_context.minor_version);
    SDL_assert_always(r);

    using gl_context_t = std::unique_ptr<std::remove_pointer_t<SDL_GLContext>,
                                         decltype(&SDL_GL_DestroyContext)>;
    gl_context_t gl_context(
        SDL_GL_CreateContext(window.get()), SDL_GL_DestroyContext);
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
    SDL_assert_always(result);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                 &got_context.minor_version);
    SDL_assert_always(result);

    clog << "Ask for " << ask_context << endl;
    clog << "Receive " << got_context << endl;

    clog << "default ";
    print_view_port();

    gles30::texture diffuse_map(fs::path{ "./res/container2.png" });
    gles30::texture specular_map(fs::path{ "./res/container2_specular.png" });

    gles30::shader material(fs::path{ "./res/vertex_pos.vsh" },
                            "./res/material.fsh");
    gles30::shader light_shader(fs::path{ "./res/vertex_pos.vsh" },
                                "./res/lamp_color.fsh");

    // Generate VAO VertexArrayState object to remember current VBO and
    // EBO(if any) with all attributes parameters stored in one object
    // called VAO think it is current VBO + EBO + attributes state in one
    // object
    uint32_t object_VAO;
    glGenVertexArrays(1, &object_VAO);
    gl_check();

    glBindVertexArray(object_VAO);
    gl_check();

    // generate OpenGL object id for future VertexBufferObject
    uint32_t VBO;
    glGenBuffers(1, &VBO);
    gl_check();

    // GL_ARRAY_BUFFER - is VertexBufferObject type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl_check();

    // copy vertex data into GPU memory
    // Using newly created VBO
    //
    // GL_STATIC_DRAW: the data will most likely not change at all or
    //    very rarely.
    // GL_DYNAMIC_DRAW: the data is likely to change a lot.
    // GL_STREAM_DRAW: the data will change every time it is drawn.
    uint32_t cube_indexes[36];
    std::iota(begin(cube_indexes), end(cube_indexes), 0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl_check();

    uint32_t EBO; // ElementBufferObject - indices buffer
    glGenBuffers(1, &EBO);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(cube_indexes),
                 cube_indexes,
                 GL_STATIC_DRAW);
    gl_check();

    update_vertex_attributes();

    uint32_t light_VAO;
    glGenVertexArrays(1, &light_VAO);
    gl_check();
    glBindVertexArray(light_VAO);
    gl_check();
    // we only need to bind to the VBO, the container's VBO's data already
    // contains the correct data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl_check();
    // set the vertex attributes (only position data for our lamp)
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), nullptr);
    gl_check();
    glEnableVertexAttribArray(0);
    gl_check();

    // indexes should be same for light box too just bind it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

    GLenum primitive_render_mode = GL_TRIANGLES;

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    camera = fps_camera(/*pos*/ { 0, 0, 1 },
                        /*dir*/ { 0, 0, -1 },
                        /*up*/ { 0, 1, 0 });

    fovy = properties.get_float("fovy");
    camera.fovy(fovy);
    aspect = properties.get_float("aspect");
    camera.aspect(aspect);

    glEnable(GL_DEPTH_TEST);
    gl_check();

    bool continue_loop = true;
    while (continue_loop)
    {
        float currentFrame = SDL_GetTicks() * 0.001f; // seconds
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        properties.update_changes();

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
                const float delta_pitch = -1 * event.motion.yrel * sensivity;
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
                        glViewport(
                            0, 0, event.window.data1, event.window.data2);
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

        float red   = 0.f;
        float green = 0.f;
        float blue  = 0.f;
        float alpha = 0.f;

        glClearColor(red, green, blue, alpha);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        light_pos = properties.get_vec3("light_pos");

        // render object
        {
            glBindVertexArray(object_VAO);
            gl_check();

            // enable new shader program
            material.use();

            light_ambient       = properties.get_vec3("light_ambient");
            light_diffuse       = properties.get_vec3("light_diffuse");
            light_specular      = properties.get_vec3("light_specular");
            glm::vec3 light_dir = camera.direction();
            light_pos           = camera.position();

            material.set_uniform("light.ambient", light_ambient);
            material.set_uniform("light.diffuse", light_diffuse);
            material.set_uniform("light.specular", light_specular);
            material.set_uniform("light.direction", light_dir);
            material.set_uniform("light.position", light_pos);
            material.set_uniform("light.cut_off",
                                 glm::cos(glm::radians(12.5f)));
            material.set_uniform("light.outer_cut_off",
                                 glm::cos(glm::radians(17.5f)));

            material.set_uniform("light.constant", 1.0f);
            material.set_uniform("light.linear", 0.09f);
            material.set_uniform("light.quadratic", 0.032f);

            material.set_uniform("viewPos", camera.position());

            material_shininess = properties.get_float("material_shininess");
            material_specular  = properties.get_vec3("material_specular");

            material.set_uniform("material.shininess", material_shininess);
            material.set_uniform("material.diffuse", diffuse_map, 0);
            material.set_uniform("material.specular", specular_map, 1);

            glm::mat4 rotated_model{ model };
            angle += properties.get_float("angle");
            rotate_axis = properties.get_vec3("rotate_axis");

            rotated_model = glm::rotate(rotated_model, angle, rotate_axis);

            material.set_uniform("model", rotated_model);
            material.set_uniform("view", view);
            material.set_uniform("projection", projection);

            glDrawElements(primitive_render_mode, 36, GL_UNSIGNED_INT, nullptr);
            gl_check();
        }

        SDL_GL_SwapWindow(window.get());
    }

    SDL_Quit();

    return 0;
}

// clang-format off
float vertices[36 * 8] = {
   // positions          // normals           // texture coords
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
