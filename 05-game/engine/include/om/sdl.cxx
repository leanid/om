#include "sdl.hxx"

#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#define FUNC_CHECK()                                                           \
    {                                                                          \
        std::string error(__FUNCTION__);                                       \
        error += "() is not implemented";                                      \
        throw std::runtime_error(error);                                       \
    }

namespace om
{

class window::impl
{
public:
    impl()                       = default;
    impl(const impl&)            = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(impl&&)      = delete;
    impl(impl&&)                 = delete;

    SDL_Window* window = nullptr;
    ~impl();
};

window::impl::~impl()
{
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

window::window(window&& w)
    : data(std::move(w.data))
// TODO Implement this
{
    throw std::runtime_error("Not implemented yet");
}

window::window(const char* title, size size, position pos, mode mod)
    : data(std::make_unique<impl>())
{
    uint32_t flags = 0;
    switch (mod)
    {
        case window::mode::opengl:
            flags |= SDL_WINDOW_OPENGL;
            break;
        case window::mode::vulkan:
            flags |= SDL_WINDOW_VULKAN;
            break;
        default:
            break;
    }
    data->window = SDL_CreateWindow(title, pos.x, pos.y, size.w, size.h, flags);
    if (!data->window)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

window::~window() {}

bool window::set_display_mode(const display_mode& om_display_mode)
{
    SDL_DisplayMode sdl_display_mode;
    sdl_display_mode.h            = om_display_mode.h;
    sdl_display_mode.w            = om_display_mode.h;
    sdl_display_mode.format       = om_display_mode.format;
    sdl_display_mode.refresh_rate = om_display_mode.refresh_rate;
    sdl_display_mode.driverdata   = nullptr;

    if (SDL_SetWindowDisplayMode(data->window, &sdl_display_mode))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

display_mode window::get_display_mode() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_DisplayMode sdl_display_mode;

    if (SDL_GetWindowDisplayMode(data->window, &sdl_display_mode))
    {
        throw std::runtime_error(SDL_GetError());
    }
    display_mode om_display_mode;
    om_display_mode.h            = sdl_display_mode.h;
    om_display_mode.w            = sdl_display_mode.h;
    om_display_mode.format       = sdl_display_mode.format;
    om_display_mode.refresh_rate = sdl_display_mode.refresh_rate;
    return om_display_mode;
}

std::uint32_t window::get_pixel_format() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    std::uint32_t result = SDL_GetWindowPixelFormat(data->window);
    if (result == SDL_PIXELFORMAT_UNKNOWN)
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void window::set_title(const char* title)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowTitle(data->window, title);
}

std::string_view window::get_title() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    const char* title = SDL_GetWindowTitle(data->window);
    if (title)
    {
        // Make sure it's not temporary.
        return { title };
    }
    return {};
}

// void window::set_icon(const surface& icon) {}  TODO Implement om::surface

void* window::set_data(const char* name, void* userdata)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    return SDL_SetWindowData(data->window, name, userdata);
}

void* window::get_data(const char* name) const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    return SDL_GetWindowData(data->window, name);
}

void window::set_position(const position& pos)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowPosition(data->window, pos.x, pos.y);
}

om::window::position window::get_position() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    position result;
    SDL_GetWindowPosition(data->window, &result.x, &result.y);
    return result;
}

void window::set_size(const size& s)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowSize(data->window, s.w, s.h);
}

om::window::size window::get_size() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);
    return size(w, h);
}

std::optional<rect> window::get_border_size() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    // supposed x,y - left upper corner coords
    int top, left, bottom, right;
    if (SDL_GetWindowBordersSize(data->window, &top, &left, &bottom, &right))
    {
        return {};
    }
    return { rect(left, top, right - left, bottom - top) };
}

void window::set_minimal_size(const size& s)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowMinimumSize(data->window, s.w, s.h);
}

window::size window::get_minimal_size() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    int w, h;
    SDL_GetWindowMinimumSize(data->window, &w, &h);
    return size(w, h);
}

void window::set_maximum_size(const size& s)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowMaximumSize(data->window, s.w, s.h);
}

window::size window::get_maximum_size() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    int w, h;
    SDL_GetWindowMaximumSize(data->window, &w, &h);
    return size(w, h);
}

void window::set_bordered(bool value)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowBordered(data->window, SDL_bool(value));
}

bool window::is_bordered() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return !(flags & SDL_WINDOW_BORDERLESS);
}

void window::set_resizable(bool value)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowResizable(data->window, SDL_bool(value));
}

bool window::is_resizeable() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_RESIZABLE);
}

void window::show()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_ShowWindow(data->window);
}

bool window::is_shown() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_SHOWN);
}

void window::hide()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_HideWindow(data->window);
}

bool window::is_hidden() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_HIDDEN);
}

void window::raise()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_RaiseWindow(data->window);
}
void window::maximize()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_MaximizeWindow(data->window);
}

bool window::is_maximized() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_MAXIMIZED);
}

void window::minimize()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_MinimizeWindow(data->window);
}

bool window::is_minimized() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_MINIMIZED);
}

void window::restore()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_RestoreWindow(data->window);
}

void window::set_fullscreen()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    if (SDL_SetWindowFullscreen(data->window, SDL_WINDOW_FULLSCREEN))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool window::is_fullscreen() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_FULLSCREEN);
}

void window::set_fullscreen_desktop()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    if (SDL_SetWindowFullscreen(data->window, SDL_WINDOW_FULLSCREEN_DESKTOP))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool window::is_fullscreen_desktop() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void window::set_windowed()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    if (SDL_SetWindowFullscreen(data->window, 0))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool window::is_windowed() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    return (!is_fullscreen_desktop());
}

// surface              get_surface() const;  TODO Implement om::surface
// bool                 update_surface(const surface&);
// bool                 update_surface_rects(const std::vector<rect>& rects);

void window::grab_input(bool value)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_SetWindowGrab(data->window, SDL_bool(value));
}

bool window::has_input_grabbed() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    return SDL_GetWindowGrab(data->window);
}

bool window::set_brightness(const float& brightness)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    // Do we need to check range of input parameter?
    // value to set where 0.0 is completely dark and 1.0 is normal brightness
    if (SDL_SetWindowBrightness(data->window, brightness))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

float window::get_brightness() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    return { SDL_GetWindowBrightness(data->window) };
}

bool window::set_opacity(const float& opacity)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    // Do we need to check range of input parameter?
    // the opacity value (0.0f - transparent, 1.0f -
    // opaque)
    if (SDL_SetWindowOpacity(data->window, opacity))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}
std::optional<float> window::get_opacity() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    float result;
    if (SDL_GetWindowOpacity(data->window, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return { result };
}

bool window::set_modal_for(window& parent)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    if (SDL_SetWindowModalFor(data->window, parent.data->window))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

void window::set_input_focus()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    if (SDL_SetWindowInputFocus(data->window))
    {
        throw std::runtime_error(SDL_GetError());
    };
    // FIXME AS SDL2 docs says You almost certainly want
    // SDL_RaiseWindow() instead of this function. Use this with
    // caution, as you might give focus to a window that is
    // completely obscured by other windows.
    return;
}

bool window::has_input_focus() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_INPUT_FOCUS);
}

bool window::has_mouse_focus() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_MOUSE_FOCUS);
}

bool window::has_mouse_captured() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_MOUSE_CAPTURE);
}

bool window::is_always_ontop() const
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint32_t flags = SDL_GetWindowFlags(data->window);
    return (flags & SDL_WINDOW_ALWAYS_ON_TOP);
}

bool window::set_gamma_ramp(
    const std::optional<std::array<std::uint16_t, 256>*> red,
    const std::optional<std::array<std::uint16_t, 256>*> green,
    const std::optional<std::array<std::uint16_t, 256>*> blue)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    const uint16_t* r = nullptr;
    const uint16_t* g = nullptr;
    const uint16_t* b = nullptr;
    if (red)
    {
        r = red.value()->data();
    }
    if (green)
    {
        g = green.value()->data();
    }
    if (blue)
    {
        b = blue.value()->data();
    }
    if (SDL_SetWindowGammaRamp(data->window, r, g, b))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

bool window::get_gamma_ramp(
    std::optional<std::array<std::uint16_t, 256>*> red,
    std::optional<std::array<std::uint16_t, 256>*> green,
    std::optional<std::array<std::uint16_t, 256>*> blue)
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    uint16_t* r = nullptr;
    uint16_t* g = nullptr;
    uint16_t* b = nullptr;
    if (red)
    {
        r = red.value()->data();
    }
    if (green)
    {
        g = green.value()->data();
    }
    if (blue)
    {
        b = blue.value()->data();
    }

    if (SDL_GetWindowGammaRamp(data->window, r, g, b))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

void window::close()
{
    if (!data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    data.reset();
}

bool window::size::operator==(const size& right) const
{
    return (this->h == right.h && this->w == right.w);
}

bool window::position::operator==(const position& right) const
{
    return (this->x == right.x && this->y == right.y);
}

class gl_context::impl
{
public:
    SDL_GLContext context = nullptr;
    ~impl(){};
};

gl_context::gl_context()
{
    data = new impl;
}

gl_context::gl_context(gl_context&& ctx)
    : data(std::move(ctx.data))
// TODO Implement this
{
    FUNC_CHECK();
}

gl_context::~gl_context()
{
    if (data)
        delete data;
}

std::vector<std::string_view> video::get_drivers()
{
    std::vector<std::string_view> result;
    int                           count = SDL_GetNumVideoDrivers();
    for (auto i = 0; i < count; ++i)
    {
        std::string_view tmp = SDL_GetVideoDriver(i);
        result.push_back(tmp);
    }
    return result;
}

void video::init(const char* driver_name)
{
    if (int error = SDL_VideoInit(driver_name); 0 != error)
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

void video::quit()
{
    SDL_VideoQuit();
    return;
}

window video::create_window(
    const char*                     title,
    const window::size              window_size,
    std::optional<window::position> window_position = {},
    std::optional<window::mode>     window_mode     = {})
{
    int x = SDL_WINDOWPOS_UNDEFINED;
    int y = SDL_WINDOWPOS_UNDEFINED;
    if (window_position)
    {
        x = window_position->x;
        y = window_position->y;
    }
    window::mode mode_ = window::mode::undefined;
    if (window_mode)
    {
        mode_ = window_mode.value();
    }
    window result(title, window_size, { x, y }, mode_);
    return result;
}

gl_context video::gl_create_context(const window& window)
{
    if (!window.data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    gl_context result;
    result.data->context = SDL_GL_CreateContext(window.data->window);
    if (!result.data->context)
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_delete_context(const gl_context& context)
{
    SDL_GL_DestroyContext(context.data->context);
    return;
}

void video::gl_swap_window(const window& window)
{
    if (!window.data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    SDL_GL_SwapWindow(window.data->window);
}

bool video::gl_make_current(const window& window, const gl_context& context)
{
    if (!window.data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    if (SDL_GL_MakeCurrent(window.data->window, context.data->context))
        return true;
    return false;
}

window::size video::gl_get_drawable_size(const window& window)
{
    if (!window.data)
    {
        throw std::runtime_error(
            "Window is not valid. It might had already been closed");
    }
    int h, w = 0;
    SDL_GL_GetDrawableSize(window.data->window, &w, &h);
    return { (size_t)w, (size_t)h };
}

std::string_view video::get_current_driver() const
{
    FUNC_CHECK();
}

std::vector<display> video::get_displays() const
{
    FUNC_CHECK();
}

display video::get_display_for_window(const window&) const
{
    FUNC_CHECK();
}

window video::create_window_from(const void* native_handle)
{
    FUNC_CHECK();
}

window video::get_window_fromID(std::uint32_t id) const
{
    FUNC_CHECK();
}

std::optional<window> video::get_grabbed_window() const
{
    FUNC_CHECK();
}

bool video::is_screen_saver_enabled() const
{
    FUNC_CHECK();
}

void video::enable_screen_saver()
{
    FUNC_CHECK();
}

void video::disable_screen_saver()
{
    FUNC_CHECK();
}

bool video::gl_load_library(std::string_view path)
{
    FUNC_CHECK();
}

void* video::gl_get_proc_address(std::string_view proc)
{
    FUNC_CHECK();
}

void video::gl_unload_library()
{
    FUNC_CHECK()
}

bool video::gl_extension_supported(std::string_view extension)
{
    FUNC_CHECK();
}

void video::gl_reset_attributes(){ FUNC_CHECK() }

window video::gl_get_current_window(){ FUNC_CHECK() }

gl_context video::gl_get_current_context()
{
    FUNC_CHECK()
}

bool video::gl_set_swap_interval(gl_swap_interval mode)
{
    int result = 0;
    switch (mode)
    {
        case gl_swap_interval::immediate:
            result = SDL_GL_SetSwapInterval(0);
            break;
        case gl_swap_interval::adaptive:
            result = SDL_GL_SetSwapInterval(-1);
            break;
        case gl_swap_interval::synchronized:
            result = SDL_GL_SetSwapInterval(1);
            break;
        default:
            break;
    }
    if (result)
    {
        return false;
    }
    return true;
}

void video::gl_attribute::red_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_RED_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::red_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::blue_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::blue_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::green_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::green_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::alpha_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::alpha_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::buffer_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::buffer_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::doublebuffer(bool val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool video::gl_attribute::doublebuffer()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::depth_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::depth_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::stencil_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::stencil_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::accum_red_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::accum_red_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::accum_green_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::accum_green_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::accum_blue_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::accum_blue_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::accum_alpha_size(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::accum_alpha_size()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::stereo(bool val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_STEREO, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool video::gl_attribute::stereo()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_STEREO, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::multisamplebuffers(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::multisamplebuffers()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::multisamplesamples(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::multisamplesamples()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::accelerated_visual(bool val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool video::gl_attribute::accelerated_visual()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::context_major_version(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::context_major_version()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::context_minor_version(int val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

int video::gl_attribute::context_minor_version()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::context_flag(gl_context_flag om_flag, bool state)
{
    int current_flags{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &current_flags))
    {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_GLcontextFlag sdl_flag;
    switch (om_flag)
    {
        case video::gl_context_flag::debug:
            sdl_flag = SDL_GL_CONTEXT_DEBUG_FLAG;
            break;
        case video::gl_context_flag::forward_compatible_mode:
            sdl_flag = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
            break;
        case video::gl_context_flag::robust_access_mode:
            sdl_flag = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
            break;
        case video::gl_context_flag::reset_isolation_mode:
            sdl_flag = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG;
            break;
        default:
            throw std::runtime_error("Never supposed to be here");
            break;
    }

    if (state)
    {
        current_flags |= sdl_flag; // turn on
    }
    else
    {
        current_flags &= ~(sdl_flag); // turn off
    }

    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, current_flags))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool video::gl_attribute::context_flag(gl_context_flag om_flag)
{
    int current_flags{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &current_flags))
    {
        throw std::runtime_error(SDL_GetError());
    }
    int sdl_flag;
    switch (om_flag)
    {
        case video::gl_context_flag::debug:
            sdl_flag = SDL_GL_CONTEXT_DEBUG_FLAG;
            break;
        case video::gl_context_flag::forward_compatible_mode:
            sdl_flag = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
            break;
        case video::gl_context_flag::robust_access_mode:
            sdl_flag = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
            break;
        case video::gl_context_flag::reset_isolation_mode:
            sdl_flag = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG;
            break;
        default:
            throw std::runtime_error("Never supposed to be here");
            break;
    }
    return current_flags & sdl_flag;
}

void video::gl_attribute::context_profile_mask(gl_context_profile profile)
{
    int sdl_profile{};
    switch (profile)
    {
        case video::gl_context_profile::automatic:
            sdl_profile = 0;
            break;
        case video::gl_context_profile::compatibility:
            sdl_profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
            break;
        case video::gl_context_profile::core:
            sdl_profile = SDL_GL_CONTEXT_PROFILE_CORE;
            break;
        case video::gl_context_profile::es:
            sdl_profile = SDL_GL_CONTEXT_PROFILE_ES;
            break;
        default:
            throw std::runtime_error("Never supposed to be here");
            break;
    }
    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, sdl_profile))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

video::gl_context_profile video::gl_attribute::context_profile_mask()
{
    video::gl_context_profile om_profile;
    int                       sdl_profile{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &sdl_profile))
    {
        throw std::runtime_error(SDL_GetError());
    }
    switch (sdl_profile)
    {
        case 0:
            om_profile = video::gl_context_profile::automatic;
            break;
        case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY:
            om_profile = video::gl_context_profile::compatibility;
            break;
        case SDL_GL_CONTEXT_PROFILE_CORE:
            om_profile = video::gl_context_profile::core;

            break;
        case SDL_GL_CONTEXT_PROFILE_ES:
            om_profile = video::gl_context_profile::es;
            break;
        default:
            throw std::runtime_error("Never supposed to be here");
            break;
    }

    return om_profile;
}

void video::gl_attribute::share_with_curent_context(bool val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool video::gl_attribute::share_with_curent_context()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::gl_framebuffer_srgb_capable(bool val)
{
    if (0 != SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, val))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

bool video::gl_attribute::gl_framebuffer_srgb_capable()
{
    int result{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

void video::gl_attribute::context_release_behavior(gl_context_release_flag flag)
{
    int sdl_flag{};
    switch (flag)
    {
        case video::gl_context_release_flag::flush:
            sdl_flag = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH;
            break;
        case video::gl_context_release_flag::none:
            sdl_flag = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE;
            break;
        default:
            throw std::runtime_error("Never supposed to be here");
            break;
    }
    if (0 != SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, sdl_flag))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return;
}

video::gl_context_release_flag video::gl_attribute::context_release_behavior()
{
    int sdl_flag{};
    if (0 != SDL_GL_GetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, &sdl_flag))
    {
        throw std::runtime_error(SDL_GetError());
    }
    switch (sdl_flag)
    {
        case SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH:
            return video::gl_context_release_flag::flush;
            break;
        case SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE:
            return video::gl_context_release_flag::none;
            break;
        default:
            throw std::runtime_error("Never supposed to be here");
            break;
    }
}

} // namespace om
