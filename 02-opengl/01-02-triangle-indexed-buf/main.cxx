#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

#include "opengles30.hxx"

struct context_parameters
{
    const char* name          = nullptr;
    int32_t     major_version = 0;
    int32_t     minor_version = 0;
    int32_t     profile_type  = 0;
};

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
    const bool init_result = SDL_Init(SDL_INIT_VIDEO);
    if (!init_result)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        return -1;
    }

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow("1-triangles, 2-lines, 3-line-strip, 4-line-loop",
                         640,
                         480,
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
    if (platform_name == "Windows"s || platform_name == "Mac OS X"s)
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

    const char* vertex_shader_src =
        "#version 300 es\n"
        "layout (location = 0) in vec4 a_position;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(a_position.x, a_position.y, \n"
        "a_position.z, 1.0);\n"
        "}\n";

    // create OpenGL object id for vertex shader object
    uint32_t vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    gl_check();

    // load vertex shader source code into vertex_shader
    glShaderSource(vertex_shader, 1, &vertex_shader_src, nullptr);
    gl_check();

    // compile vertex shader
    glCompileShader(vertex_shader);
    gl_check();

    // check compilation status of our shader
    int  success;
    char info_log[1024] = { 0 };
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);
        gl_check();

        clog << "error: in vertex shader: " << info_log << endl;
        exit(-1);
    }

    const char* fragment_shader_src =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 frag_color;\n"
        "void main()\n"
        "{\n"
        "    frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

    // generate new id for shader object
    uint32_t fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    gl_check();
    // load fragment shader source code
    glShaderSource(fragment_shader, 1, &fragment_shader_src, nullptr);
    gl_check();

    glCompileShader(fragment_shader);
    gl_check();

    // check compilation status of our shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetShaderInfoLog(
            fragment_shader, sizeof(info_log), nullptr, info_log);
        gl_check();

        clog << "error: in fragment shader: " << info_log << endl;
        exit(-1);
    }

    // create complete shader program and reseive id (vertex + geometry +
    // fragment) geometry shader - will have default value
    uint32_t shader_program;
    shader_program = glCreateProgram();
    gl_check();

    glAttachShader(shader_program, vertex_shader);
    gl_check();

    glAttachShader(shader_program, fragment_shader);
    gl_check();

    // no link program like in c/c++ object files
    glLinkProgram(shader_program);
    gl_check();

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    gl_check();

    if (0 == success)
    {
        glGetProgramInfoLog(
            shader_program, sizeof(info_log), nullptr, info_log);
        gl_check();

        clog << "error: linking: " << info_log << endl;
        exit(-1);
    }

    // after linking shader program we don't need object parts of it
    // so we can free OpenGL memory and delete vertex and fragment parts
    glDeleteShader(vertex_shader);
    gl_check();
    glDeleteShader(fragment_shader);
    gl_check();

    float vertices[] = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
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

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

    // now tell OpenGL how to interpret data from VBO
    int location_of_vertex_attribute = 0;
    int size_of_attribute            = 3; // 3 float values (x, y, z)
    int type_of_data   = GL_FLOAT;        // all values in vec{2,3,4} of float
    int normalize_data = GL_FALSE;        // OpenGL can normalize values
    // to [0, 1] - for unsigned and to [-1, 1] for signed values
    int stride = 3 * sizeof(float); // step in bytes from one attribute to next
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

    int primitive_render_mode = GL_TRIANGLES;

    bool continue_loop = true;
    while (continue_loop)
    {
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
            }
            else if (SDL_EVENT_WINDOW_RESIZED == event.type)
            {
                clog << "windows resized: " << event.window.data1 << ' '
                     << event.window.data2 << ' ';
                // play with it to understand OpenGL origin point
                // for window screen coordinate system
                glViewport(
                    0, 0, event.window.data1, event.window.data2);
                gl_check();
                print_view_port();
            }
        }

        float red   = 0.f;
        float green = 1.f;
        float blue  = 0.f;
        float alpha = 0.f;

        glClearColor(red, green, blue, alpha);

        glClear(GL_COLOR_BUFFER_BIT);

        // enable new shader program
        glUseProgram(shader_program);
        gl_check();

        // one call select VBO and all attributes like we setup before
        glBindVertexArray(VAO);
        gl_check();

        glDrawElements(primitive_render_mode, 6, GL_UNSIGNED_INT, 0);
        gl_check();

        SDL_GL_SwapWindow(window.get());
    }

    SDL_Quit();

    return 0;
}
