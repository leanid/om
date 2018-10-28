#include "sdl.hxx"

#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

namespace om
{

class window::impl
{
public:
    impl()            = default;
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(impl&&) = delete;
    impl(impl&&)            = delete;

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
    throw std::runtime_error("Not implemented yet");
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
    const char* title, const window::size window_size,
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
    SDL_GL_DeleteContext(context.data->context);
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

} // namespace om
