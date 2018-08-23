#include "sdl.hxx"
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <stdexcept>

namespace om
{

class window::impl
{
public:
    SDL_Window* pwindow = nullptr;
};

window::window(std::string_view title, size size, std::optional<position> pos,
               std::optional<uint32_t> om_flags)
{
    pImpl = new window::impl;
    if (title.empty())
    {
        title = "noname window";
    }
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
    SDL_Window* tmp =
        SDL_CreateWindow(title.data(), x, y, size.w, size.h, sdl_flags);
    if (tmp)
        pImpl->pwindow = tmp;
    else
    {
        delete pImpl;
        pImpl = nullptr;
    }
}

window::~window()
{
    if (pImpl)
    {
        // Internal SDL_Window pointer is not checked here.
        // It's supposed that if pImpl is not nullptr then
        // there is no way SDL_Window == nullptr;
        SDL_DestroyWindow(pImpl->pwindow);
        delete pImpl;
        pImpl = nullptr;
    }
}

bool window::set_display_mode(const display_mode& om_display_mode)
{
    SDL_DisplayMode sdl_display_mode;
    sdl_display_mode.h            = om_display_mode.h;
    sdl_display_mode.w            = om_display_mode.h;
    sdl_display_mode.format       = om_display_mode.format;
    sdl_display_mode.refresh_rate = om_display_mode.refresh_rate;
    sdl_display_mode.driverdata   = nullptr;

    if (SDL_SetWindowDisplayMode(pImpl->pwindow, &sdl_display_mode))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
    }
    return true;
}

display_mode window::get_display_mode() const
{
    SDL_DisplayMode sdl_display_mode;

    if (SDL_GetWindowDisplayMode(pImpl->pwindow, &sdl_display_mode))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return {};
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
    std::uint32_t result = SDL_GetWindowPixelFormat(pImpl->pwindow);
    if (result == SDL_PIXELFORMAT_UNKNOWN)
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return {};
    }
    return result;
}

// std::uint32_t        get_id() const; DO WE NEED IT?

std::uint32_t window::get_flags() const
{
    return SDL_GetWindowFlags(pImpl->pwindow);
}

void window::set_title(std::string_view title)
{
    if (title.empty())
        return;
    SDL_SetWindowTitle(pImpl->pwindow, title.data());
    return;
}

std::string_view window::get_title() const
{
    const char* title = SDL_GetWindowTitle(pImpl->pwindow);
    if (title)
    {
        return { title };
    }
    return {};
}

// void window::set_icon(const surface& icon) {}  TODO Implement om::surface

void* window::set_data(std::string_view name, void* userdata)
{
    if (name.empty())
    {
        return nullptr; // Is empty string invalid value?
    }
    return SDL_SetWindowData(pImpl->pwindow, name.data(), userdata);
}

void* window::get_data(std::string_view name) const
{
    if (name.empty())
        return nullptr;
    return SDL_GetWindowData(pImpl->pwindow, name.data());
}

void window::set_position(const position& pos)
{
    SDL_SetWindowPosition(pImpl->pwindow, pos.x, pos.y);
}

om::window::position window::get_position() const
{
    position result;
    SDL_GetWindowPosition(pImpl->pwindow, &result.x, &result.y);
    return result;
}

void window::set_size(const size& s)
{
    if (s.h == 0 || s.w == 0)
        return;
    SDL_SetWindowSize(pImpl->pwindow, s.w, s.h);
}

om::window::size window::get_size() const
{
    int w, h;
    SDL_GetWindowSize(pImpl->pwindow, &w, &h);
    return size(w, h);
}

std::optional<rect> window::get_border_size() const
{
    // supposed x,y - left upper corner coords
    int top, left, bottom, right;
    if (SDL_GetWindowBordersSize(pImpl->pwindow, &top, &left, &bottom, &right))
    {
        std::string_view error(SDL_GetError());
        if (error.empty())
            return {};
        else
        {
            throw std::runtime_error(error.data());
            SDL_ClearError();
            return {};
        }
    }
    rect result(left, top, right - left, bottom - top);
    return result;
}

void window::set_minimal_size(const size& s)
{
    SDL_SetWindowMinimumSize(pImpl->pwindow, s.w, s.h);
}

window::size window::get_minimal_size() const
{
    int w, h;
    SDL_GetWindowMinimumSize(pImpl->pwindow, &w, &h);
    return size(w, h);
}

void window::set_maximum_size(const size& s)
{
    SDL_SetWindowMaximumSize(pImpl->pwindow, s.w, s.h);
}

window::size window::get_maximum_size() const
{
    int w, h;
    SDL_GetWindowMaximumSize(pImpl->pwindow, &w, &h);
    return size(w, h);
}

void window::set_bordered(bool state)
{
    if (state)
        SDL_SetWindowBordered(pImpl->pwindow, SDL_TRUE);
    else
        SDL_SetWindowBordered(pImpl->pwindow, SDL_FALSE);
}

void window::set_resizable(bool state)
{
    if (state)
        SDL_SetWindowResizable(pImpl->pwindow, SDL_TRUE);
    else
        SDL_SetWindowResizable(pImpl->pwindow, SDL_FALSE);
}

void window::show()
{
    SDL_ShowWindow(pImpl->pwindow);
}

void window::hide()
{
    SDL_HideWindow(pImpl->pwindow);
}

void window::raise()
{
    SDL_RaiseWindow(pImpl->pwindow);
}
void window::maximize()
{
    SDL_MaximizeWindow(pImpl->pwindow);
}

void window::minimize()
{
    SDL_MinimizeWindow(pImpl->pwindow);
}

void window::restore()
{
    SDL_RestoreWindow(pImpl->pwindow);
}

bool window::set_fullscreen(const uint32_t& fl)
{
    if (SDL_SetWindowFullscreen(pImpl->pwindow, static_cast<uint32_t>(fl)))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
    }
    return true;
}

// surface              get_surface() const;  TODO Implement om::surface
// bool                 update_surface(const surface&);
// bool                 update_surface_rects(const std::vector<rect>& rects);

void window::set_grabbed(bool state)
{
    if (state)
        SDL_SetWindowGrab(pImpl->pwindow, SDL_TRUE);
    else
        SDL_SetWindowGrab(pImpl->pwindow, SDL_FALSE);
}

bool window::get_grabbed() const
{
    if (SDL_GetWindowGrab(pImpl->pwindow))
        return true;
    else
        return false;
}

bool window::set_brightness(const float& brightness)
{
    // Do we need to check range of input parameter?
    // value to set where 0.0 is completely dark and 1.0 is normal brightness
    if (SDL_SetWindowBrightness(pImpl->pwindow, brightness))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
    }
    return true;
}

float window::get_brightness() const
{
    return { SDL_GetWindowBrightness(pImpl->pwindow) };
}

bool window::set_opacity(const float& opacity)
{
    // Do we need to check range of input parameter?
    // the opacity value (0.0f - transparent, 1.0f -
    // opaque)
    if (SDL_SetWindowOpacity(pImpl->pwindow, opacity))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
    }
    return true;
}
std::optional<float> window::get_opacity() const
{
    float result = 1.f;
    if (SDL_GetWindowOpacity(pImpl->pwindow, &result))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return {};
    }
    return { result };
}

bool window::set_modal_for(window& parent)
{
    if (SDL_SetWindowModalFor(pImpl->pwindow, parent.pImpl->pwindow))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
    }
    return true;
}

bool window::set_input_focus()
{
    if (SDL_SetWindowInputFocus(pImpl->pwindow))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
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
    if (SDL_SetWindowGammaRamp(pImpl->pwindow, r, g, b))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
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

    if (SDL_GetWindowGammaRamp(pImpl->pwindow, r, g, b))
    {
        throw std::runtime_error(SDL_GetError());
        SDL_ClearError();
        return false;
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
        SDL_ClearError();
    }
    return;
}
void video::quit()
{
    SDL_VideoQuit();
    return;
}

} // namespace om
