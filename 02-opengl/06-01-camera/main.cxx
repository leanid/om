#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

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

extern float cube_vertices[36 * 8];

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

    location_of_vertex_attribute = 1; // color
    size_of_attribute            = 3; // r + g + b
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

    location_of_vertex_attribute = 2; // tex coord
    size_of_attribute            = 2; // u + v (s + t)
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
    gl_check();
}

void mouse_callback(float xpos, float ypos)
{
    float sensitivity = 0.05f;
    yaw += xpos * sensitivity;   // xoffset;
    pitch += ypos * sensitivity; // yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = std::sin(glm::radians(yaw));
    front.y = std::sin(glm::radians(pitch));
    front.z =
        -1.f * std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    using namespace std::chrono;

    properties_reader properties("./res/runtime.properties.hxx");

    auto start_time = high_resolution_clock::now();

    const int init_result = SDL_Init(SDL_INIT_VIDEO);
    if (init_result != 0)
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
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                            ask_context.major_version);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                            ask_context.minor_version);
    SDL_assert_always(r == 0);

    unique_ptr<void, void (*)(void*)> gl_context(
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

    // Generate VAO VertexArrayState object to remember current VBO and EBO(if
    // any) with all attributes parameters stored in one object called VAO think
    // it is current VBO + EBO + attributes state in one object
    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    gl_check();

    glBindVertexArray(VAO);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    gl_check();

    uint32_t EBO; // ElementBufferObject - indices buffer
    glGenBuffers(1, &EBO);
    gl_check();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl_check();

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    gl_check();

    update_vertex_attributes();

    GLenum primitive_render_mode = GL_TRIANGLES;

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    fovy = properties.get_float("fovy");

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
                float xpos = event.motion.xrel;
                float ypos = event.motion.yrel;
                mouse_callback(xpos, ypos);
            }
            else if (SDL_EVENT_MOUSE_WHEEL == event.type)
            {
                if (fovy >= 1.0f && fovy <= 45.0f)
                    fovy -= event.wheel.y;
                if (fovy <= 1.0f)
                    fovy = 1.0f;
                if (fovy >= 45.0f)
                    fovy = 45.0f;
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

        enable_depth = properties.get_bool("enable_depth");
        use_cube     = properties.get_bool("use_cube");
        multi_cube   = properties.get_bool("multi_cube");

        if (enable_depth)
        {
            glEnable(GL_DEPTH_TEST);
            gl_check();
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            gl_check();
        }

        if (use_cube)
        {
            uint32_t cube_indexes[36];
            std::iota(begin(cube_indexes), end(cube_indexes), 0);

            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(cube_vertices),
                         cube_vertices,
                         GL_DYNAMIC_DRAW);
            gl_check();

            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(cube_indexes),
                         cube_indexes,
                         GL_DYNAMIC_DRAW);
            gl_check();
        }
        else
        {
            glBufferData(
                GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            gl_check();

            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(indices),
                         indices,
                         GL_DYNAMIC_DRAW);
            gl_check();
        }

        auto current_time = high_resolution_clock::now();

        milliseconds now{ duration_cast<milliseconds>(current_time -
                                                      start_time) };

        float red   = 0.f;
        float green = 1.f;
        float blue  = 0.f;
        float alpha = 0.f;

        glClearColor(red, green, blue, alpha);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // enable new shader program
        shader.use();

        glm::mat4 model(1);
        angle         = properties.get_float("angle");
        rotate_axis   = properties.get_vec3("rotate_axis");
        angle_per_sec = properties.get_float("angle_per_sec");
        if (!multi_cube)
        {
            model = glm::rotate(
                model,
                glm::radians(float(now.count() * 0.001f * angle_per_sec)),
                rotate_axis);
        }

        glm::mat4 view(1.f);
        move_camera = properties.get_vec3("move_camera");
        radius      = properties.get_float("radius");
        use_wasd    = properties.get_bool("use_wasd");

        if (!use_wasd)
        {
            uint32_t time_from_init_ms = SDL_GetTicks();
            float    seconds           = time_from_init_ms * 0.001f;

            glm::vec3 camera_position{ radius * std::sin(seconds),
                                       0.f,
                                       radius * std::cos(seconds) };
            view = glm::lookAt(
                camera_position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }
        else
        {
            cameraSpeed               = properties.get_float("cameraSpeed");
            const uint8_t* keys_state = SDL_GetKeyboardState(nullptr);
            if (keys_state[SDL_SCANCODE_W])
                cameraPos += cameraSpeed * deltaTime * cameraFront;
            if (keys_state[SDL_SCANCODE_S])
                cameraPos -= cameraSpeed * deltaTime * cameraFront;
            if (keys_state[SDL_SCANCODE_A])
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
                             cameraSpeed * deltaTime;
            if (keys_state[SDL_SCANCODE_D])
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
                             cameraSpeed * deltaTime;

            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        }

        aspect = properties.get_float("aspect");
        z_near = properties.get_float("z_near"); // 3.f;
        z_far  = properties.get_float("z_far");  // 100.f;
        glm::mat4 projection =
            glm::perspective(glm::radians(fovy), aspect, z_near, z_far);

        shader.set_uniform("texture0", texture0, 0);
        shader.set_uniform("texture1", texture1, 1);
        shader.set_uniform("model", model);
        shader.set_uniform("view", view);
        shader.set_uniform("projection", projection);

        if (std::string log = shader.validate(); !log.empty())
        {
            clog << log << endl;
        }

        if (multi_cube && use_cube)
        {
            glm::vec3 cube_positions[] = {
                glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)
            };

            int i = 0;
            for (glm::vec3 pos : cube_positions)
            {
                model       = glm::translate(model, pos);
                float angle = 20.0f * i++;
                model       = glm::rotate(
                    model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                shader.set_uniform("model", model);

                glDrawElements(
                    primitive_render_mode, 36, GL_UNSIGNED_INT, nullptr);
                gl_check();
            }
        }
        else
        {
            if (use_cube)
            {
                glDrawElements(
                    primitive_render_mode, 36, GL_UNSIGNED_INT, nullptr);
                gl_check();
            }
            else
            {
                glDrawElements(
                    primitive_render_mode, 6, GL_UNSIGNED_INT, nullptr);
                gl_check();
            }
        }

        SDL_GL_SwapWindow(window.get());
    }

    SDL_Quit();

    return 0;
}

// clang-format off
float cube_vertices[36 * 8] = {
    // pos               // color          // tex coord
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f
};
// clang-format on
