#include "sdl.hxx"

#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

namespace om
{

class window::impl
{
public:
    SDL_Window* window = nullptr;
    ~impl();
};

window::impl::~impl()
{
    if (window)
    {
        SDL_DestroyWindow(window);
    }
}

window::window(window&& w)
    : data(std::move(w.data))
// TODO Implement this
{
    throw std::runtime_error("Not implemented yet");
}

window::window(const char* title, size size, std::optional<position> pos,
               std::optional<uint32_t> om_flags)
    : data(std::make_unique<impl>())
{
    uint32_t sdl_flags = 0;
    if (om_flags)
    {
        sdl_flags = static_cast<uint32_t>(om_flags.value());
    }
    int x = SDL_WINDOWPOS_UNDEFINED;
    int y = SDL_WINDOWPOS_UNDEFINED;
    if (pos)
    {
        x = pos->x;
        y = pos->y;
    }
    data->window = SDL_CreateWindow(title, x, y, size.w, size.h, sdl_flags);
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
    std::uint32_t result = SDL_GetWindowPixelFormat(data->window);
    if (result == SDL_PIXELFORMAT_UNKNOWN)
    {
        throw std::runtime_error(SDL_GetError());
    }
    return result;
}

// std::uint32_t        get_id() const; DO WE NEED IT?

std::uint32_t window::get_flags() const
{
    return SDL_GetWindowFlags(data->window);
}

void window::set_title(const char* title)
{
    if (title)
        SDL_SetWindowTitle(data->window, title);
    return;
}

std::string_view window::get_title() const
{
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
    if (name)
        return SDL_SetWindowData(data->window, name, userdata);
    return nullptr;
}

void* window::get_data(const char* name) const
{
    if (name)
        return SDL_GetWindowData(data->window, name);
    return nullptr;
}

void window::set_position(const position& pos)
{
    SDL_SetWindowPosition(data->window, pos.x, pos.y);
}

om::window::position window::get_position() const
{
    position result;
    SDL_GetWindowPosition(data->window, &result.x, &result.y);
    return result;
}

void window::set_size(const size& s)
{
    if (s.h == 0 || s.w == 0)
        return;
    SDL_SetWindowSize(data->window, s.w, s.h);
}

om::window::size window::get_size() const
{
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);
    return size(w, h);
}

std::optional<rect> window::get_border_size() const
{
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
    SDL_SetWindowMinimumSize(data->window, s.w, s.h);
}

window::size window::get_minimal_size() const
{
    int w, h;
    SDL_GetWindowMinimumSize(data->window, &w, &h);
    return size(w, h);
}

void window::set_maximum_size(const size& s)
{
    SDL_SetWindowMaximumSize(data->window, s.w, s.h);
}

window::size window::get_maximum_size() const
{
    int w, h;
    SDL_GetWindowMaximumSize(data->window, &w, &h);
    return size(w, h);
}

void window::set_bordered(bool value)
{
    SDL_SetWindowBordered(data->window, SDL_bool(value));
}

void window::set_resizable(bool value)
{
    SDL_SetWindowResizable(data->window, SDL_bool(value));
}

void window::show()
{
    SDL_ShowWindow(data->window);
}

void window::hide()
{
    SDL_HideWindow(data->window);
}

void window::raise()
{
    SDL_RaiseWindow(data->window);
}
void window::maximize()
{
    SDL_MaximizeWindow(data->window);
}

void window::minimize()
{
    SDL_MinimizeWindow(data->window);
}

void window::restore()
{
    SDL_RestoreWindow(data->window);
}

bool window::set_fullscreen(const uint32_t& flags)
{
    if (SDL_SetWindowFullscreen(data->window, static_cast<uint32_t>(flags)))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

// surface              get_surface() const;  TODO Implement om::surface
// bool                 update_surface(const surface&);
// bool                 update_surface_rects(const std::vector<rect>& rects);

void window::set_grabbed(bool value)
{
    SDL_SetWindowGrab(data->window, SDL_bool(value));
}

bool window::get_grabbed() const
{
    if (SDL_GetWindowGrab(data->window))
        return true;
    else
        return false;
}

bool window::set_brightness(const float& brightness)
{
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
    return { SDL_GetWindowBrightness(data->window) };
}

bool window::set_opacity(const float& opacity)
{
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
    float result;
    if (SDL_GetWindowOpacity(data->window, &result))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return { result };
}

bool window::set_modal_for(window& parent)
{
    if (SDL_SetWindowModalFor(data->window, parent.data->window))
    {
        throw std::runtime_error(SDL_GetError());
    }
    return true;
}

bool window::set_input_focus()
{
    if (SDL_SetWindowInputFocus(data->window))
    {
        throw std::runtime_error(SDL_GetError());
    };
    // FIXME AS SDL2 docs says You almost certainly want
    // SDL_RaiseWindow() instead of this function. Use this with
    // caution, as you might give focus to a window that is
    // completely obscured by other windows.
    return true;
}
bool window::set_gamma_ramp(
    const std::optional<std::array<std::uint16_t, 256>*> red,
    const std::optional<std::array<std::uint16_t, 256>*> green,
    const std::optional<std::array<std::uint16_t, 256>*> blue)
{
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

bool window::size::operator==(const size& right) const
{
    if (this->h == right.h && this->w == right.w)
        return true;
    return false;
}

bool window::position::operator==(const position& right) const
{
    if (this->x == right.x && this->y == right.y)
        return true;
    return false;
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

void video::init(std::string_view driver_name)
{
    const char* sz_driver_name = nullptr;
    if (!driver_name.empty())
    {
        sz_driver_name = driver_name.data();
    }
    if (int error = SDL_VideoInit(sz_driver_name); 0 != error)
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

} // namespace om
