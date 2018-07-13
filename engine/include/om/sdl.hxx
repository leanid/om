#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace om
{

struct rect
{
    std::int32_t x;
    std::int32_t y;
    std::int32_t w;
    std::int32_t h;
};

struct display_mode
{
    std::uint32_t format;
    std::int32_t  w;
    std::int32_t  h;
    std::int32_t  refresh_rate;
};

struct display
{
    std::string_view          get_name() const;
    rect                      get_bounds() const;
    float                     get_diagonal_DPI() const;
    float                     get_horizontal_DPI() const;
    float                     get_vertical_DPI() const;
    rect                      get_usable_bounds() const;
    std::vector<display_mode> get_display_modes() const;
    display_mode              get_desktop_display_mode() const;
    display_mode              get_current_display_mode() const;
    display_mode get_closest_display_mode(const display_mode&) const;
};

struct surface
{
};

struct window
{
    enum class flags
    {
        // TODO ...
    };

    static const std::int32_t centered;  // TODO
    static const std::int32_t undefined; // TODO

    struct position
    {
        std::int32_t x;
        std::int32_t y;
    };

    struct size
    {
        std::int32_t w;
        std::int32_t h;
    };

    bool                 set_display_mode(const display_mode&);
    display_mode         get_display_mode() const;
    std::uint32_t        get_pixel_format() const;
    std::uint32_t        get_id() const;
    std::uint32_t        get_flags() const;
    void                 set_title(std::string_view title);
    std::string_view     get_title() const;
    void                 set_icon(const surface& icon);
    void*                set_data(std::string_view name, void* userdata);
    void*                get_data(std::string_view name) const;
    void                 set_position(position);
    position             get_position() const;
    void                 set_size(size);
    size                 get_size() const;
    std::optional<rect>  get_border_size() const;
    void                 set_minimal_size(size);
    size                 get_minimal_size() const;
    void                 set_maximum_size(size);
    size                 get_maximum_size() const;
    void                 set_bordered(bool);
    void                 set_resizable(bool);
    void                 show();
    void                 hide();
    void                 raise();
    void                 maximize();
    void                 minimize();
    void                 restore();
    bool                 set_fullscreen(flags);
    surface              get_surface() const;
    bool                 update_surface(const surface&);
    bool                 update_surface_rects(const std::vector<rect>& rects);
    void                 set_grabbed(bool);
    bool                 get_grabbed() const;
    bool                 set_brightness(float brightness);
    float                get_brightness() const;
    bool                 set_opacity(float opacity);
    std::optional<float> get_opacity() const;
    bool                 set_modal_for(window& parent);
    bool                 set_input_focus();
    bool                 set_gamma_ramp(
                        const std::optional<std::array<std::uint16_t, 256>*> red,
                        const std::optional<std::array<std::uint16_t, 256>*> green,
                        const std::optional<std::array<std::uint16_t, 256>*> blue);
    bool get_gamma_ramp(std::optional<std::array<std::uint16_t, 256>*> red,
                        std::optional<std::array<std::uint16_t, 256>*> green,
                        std::optional<std::array<std::uint16_t, 256>*> blue);
};

struct video
{
    std::vector<std::string_view> get_drivers() const;
    void                          init(std::string_view driver_name);
    void                          quit();
    std::string_view              get_current_driver() const;

    std::vector<display> get_displays() const;
    display              get_display_for_window(const window&) const;

    window create_window(std::string_view title, std::int32_t x, std::int32_t y,
                         std::int32_t w, std::int32_t h, window::flags flags);
    window create_window_from(const void* native_handle);
    window get_window_fromID(std::uint32_t id) const;
    std::optional<window> get_grabbed_window() const;
};

struct sdlxx
{
    explicit sdlxx();
    ~sdlxx();
    sdlxx(const sdlxx&) = delete;
    sdlxx& operator=(const sdlxx&) = delete;

}; // end sdlxx
} // end namespace om
