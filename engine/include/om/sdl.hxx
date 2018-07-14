#pragma once

#include <any>
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

enum class hittest_result
{
    normal,
    draggable,
    resize_topleft,
    resize_top,
    resize_topright,
    resize_right,
    resize_bottomright,
    resize_bottom,
    resize_bottomleft,
    resize_left
};

hittest_result hit_test(window& win, const point& p, std::any);

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
    bool set_hit_test(hit_test callback, std::any);
};

enum class gl_attribute
{
    gl_red_size,
    gl_green_size,
    gl_blue_size,
    gl_alpha_size,
    gl_buffer_size,
    gl_doublebuffer,
    gl_depth_size,
    gl_stencil_size,
    gl_accum_red_size,
    gl_accum_green_size,
    gl_accum_blue_size,
    gl_accum_alpha_size,
    gl_stereo,
    gl_multisamplebuffers,
    gl_multisamplesamples,
    gl_accelerated_visual,
    gl_retained_backing,
    gl_context_major_version,
    gl_context_minor_version,
    gl_context_egl,
    gl_context_flags,
    gl_context_profile_mask,
    gl_share_with_current_context,
    gl_framebuffer_srgb_capable,
    gl_context_release_behavior,
    gl_context_reset_notification,
    gl_context_no_error
};

struct gl_context
{
};

enum class gl_swap_interval
{
    immediate,
    synchronized,
    late_swaps
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
    void   destroy_window(window&);
    window get_window_fromID(std::uint32_t id) const;
    std::optional<window> get_grabbed_window() const;
    bool                  is_screen_saver_enabled() const;
    void                  enable_screen_saver();
    void                  disable_screen_saver();

    bool         gl_load_library(std::string_view path);
    void*        gl_get_proc_address(std::string_view proc);
    void         gl_unload_library();
    bool         gl_extension_supported(std::string_view extension);
    void         gl_reset_attributes();
    bool         gl_set_attribute(gl_attribute attr, std::int32_t value);
    std::int32_t gl_get_attribute(gl_attribute attr);
    gl_context   gl_create_context();
    bool         gl_make_current(gl_context context);
    window       gl_get_current_window();
    gl_context   gl_get_current_context();
    size         gl_get_drawable_size(window wnd) const;
    bool         gl_set_swap_interval(gl_swap_interval);
    void         gl_swap_window(window wnd);
    void         gl_delete_context(gl_context);
};

struct sdlxx
{
    explicit sdlxx();
    ~sdlxx();
    sdlxx(const sdlxx&) = delete;
    sdlxx& operator=(const sdlxx&) = delete;

}; // end sdlxx
} // end namespace om
