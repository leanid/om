#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "gles30_shader.hxx"
#include "gles30_texture.hxx"
#include "opengles30.hxx"
#include "properties_reader.hxx"

#include "res/runtime.properties.hxx"

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

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    using namespace std::chrono;

    properties_reader properties("./res/runtime.properties.hxx");

    auto start_time = high_resolution_clock::now();

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        return -1;
    }

    const std::string title = properties.get_string("title");

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
    SDL_assert_always(result == 0);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                 &got_context.minor_version);
    SDL_assert_always(result == 0);

    clog << "Ask for " << ask_context << endl;
    clog << "Receive " << got_context << endl;

    clog << "default ";
    print_view_port();

    fs::path vertex_path{ "./res/basic.vsh" };
    fs::path fragment_path{ "./res/basic.fsh" };

    gles30::shader  shader(vertex_path, fragment_path);
    gles30::texture texture0(fs::path("./res/1.jpg"));
    gles30::texture texture1(fs::path("./res/2.jpg"));

    float vertices[] = {
        // pos              // color       // tex coord
        0.5f,  0.5f,  0.0f, 1.f, 1.f, 1.f, 1.f, 1.f, // top right
        0.5f,  -0.5f, 0.0f, 1.f, 1.f, 1.f, 1.f, 0.f, // bottom right
        -0.5f, -0.5f, 0.0f, 1.f, 1.f, 1.f, 0.f, 0.f, // bottom left
        -0.5f, 0.5f,  0.0f, 1.f, 1.f, 1.f, 0.f, 1.f  // top left
    };

    uint32_t indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // generate OpenGL object id for future VertexBufferObject
    uint32_t VBO;
    glGenBuffers(1, &VBO);
    gl_check();

    // Generate VAO VertexArrayState object to remember current VBO and EBO(if
    // any) with all attributes parameters stored in one object called VAO think
    // it is current VBO + EBO + attributes state in one object
    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    gl_check();

    glBindVertexArray(VAO);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl_check();

    uint32_t EBO; // ElementBufferObject - indices buffer
    glGenBuffers(1, &EBO);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

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

    location_of_vertex_attribute = 1; // color
    size_of_attribute            = 3; // r + g + b
    type_of_data                 = GL_FLOAT;
    normalize_data               = GL_FALSE;
    start_of_data_offset         = reinterpret_cast<void*>(3 * sizeof(float));
    glVertexAttribPointer(location_of_vertex_attribute, size_of_attribute,
                          type_of_data, normalize_data, stride,
                          start_of_data_offset);
    gl_check();

    glEnableVertexAttribArray(1);
    gl_check();

    location_of_vertex_attribute = 2; // tex coord
    size_of_attribute            = 2; // u + v (s + t)
    type_of_data                 = GL_FLOAT;
    normalize_data               = GL_FALSE;
    start_of_data_offset         = reinterpret_cast<void*>(6 * sizeof(float));
    glVertexAttribPointer(location_of_vertex_attribute, size_of_attribute,
                          type_of_data, normalize_data, stride,
                          start_of_data_offset);
    gl_check();

    glEnableVertexAttribArray(2);
    gl_check();

    GLenum primitive_render_mode = GL_TRIANGLES;

    bool continue_loop = true;
    while (continue_loop)
    {
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

        glm::mat4 model;
        model = glm::rotate(model, glm::radians(properties.get_float("angle")),
                            glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 view(1.f);
        view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

        fovy   = glm::radians(properties.get_float("fovy"));
        aspect = properties.get_float("aspect");
        z_near = properties.get_float("z_near"); // 3.f;
        z_far  = properties.get_float("z_far");  // 100.f;
        glm::mat4 projection;
        projection = glm::perspective(fovy, aspect, z_near, z_far);

        auto current_time = high_resolution_clock::now();

        milliseconds now{ duration_cast<milliseconds>(current_time -
                                                      start_time) };
        float        sin_value = std::sin(now.count() * 0.001f);

        glm::mat4 transform(1.f);

        transform =
            glm::rotate(transform, glm::radians(10 * now.count() * 0.001f),
                        glm::vec3(0, 0, 1.0f));

        transform = glm::scale(transform, glm::vec3(sin_value, sin_value, 1.0));

        float red   = 0.f;
        float green = 1.f;
        float blue  = 0.f;
        float alpha = 0.f;

        glClearColor(red, green, blue, alpha);

        glClear(GL_COLOR_BUFFER_BIT);

        // enable new shader program
        shader.use();

        shader.set_uniform("texture0", texture0, 0);
        shader.set_uniform("texture1", texture1, 1);
        shader.set_uniform("model", model);
        shader.set_uniform("view", view);
        shader.set_uniform("projection", projection);

        // one call select VBO and all attributes like we setup before
        glBindVertexArray(VAO);
        gl_check();

        if (std::string log = shader.validate(); !log.empty())
        {
            clog << log << endl;
        }

        glDrawElements(primitive_render_mode, 6, GL_UNSIGNED_INT, nullptr);
        gl_check();

        SDL_GL_SwapWindow(window.get());
    }

    SDL_Quit();

    return 0;
}
