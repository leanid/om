#pragma once

#include <any>
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace om
{

struct rect
{
    rect(){};
    rect(std::int32_t in_x, std::int32_t in_y, std::int32_t in_w,
         std::int32_t in_h)
        : x{ in_x }
        , y{ in_y }
        , w{ in_w }
        , h{ in_h } {};
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t w = 0;
    std::int32_t h = 0;
};

struct display_mode
{
    std::uint32_t format       = 0;
    std::int32_t  w            = 0;
    std::int32_t  h            = 0;
    std::int32_t  refresh_rate = 0;
};

struct display
{
    // TODO Implement me
    display_mode mode;
    char         name[256];
    float        ddpi;
    float        vdpi;
    float        hdpi;
    rect         bounds;
    rect         usable_bounds;

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

struct window;

struct point
{
    std::int32_t x;
    std::int32_t y;
};

hittest_result hit_test(window& win, const point& p, std::any);

class window
{
public:
    // FIXME The flags below are hardcoded
    enum class mode
    {
        undefined,
        opengl,
        vulkan
    };

    static const std::int32_t centered;  // TODO
    static const std::int32_t undefined; // TODO

    struct position
    {
        std::int32_t x = 0;
        std::int32_t y = 0;
        position(){};
        position(std::int32_t in_x, std::int32_t in_y)
            : x(in_x)
            , y(in_y)
        {
        }
        bool operator==(const position& other) const;
    };

    struct size
    {
        std::size_t w = 0;
        std::size_t h = 0;
        size(){};
        size(std::size_t in_w, std::size_t in_h)
            : w(in_w)
            , h(in_h)
        {
        }
        bool operator==(const size& other) const;
    };

public:
    window()              = delete;
    window(const window&) = delete;
    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

    window(window&&);

    bool                 set_display_mode(const display_mode&);
    display_mode         get_display_mode() const;
    std::uint32_t        get_pixel_format() const;
    std::uint32_t        get_id() const;
    std::uint32_t        get_flags() const;
    void                 set_title(const char* title);
    std::string_view     get_title() const;
    void                 set_icon(const surface& icon);
    void*                set_data(const char* name, void* userdata);
    void*                get_data(const char* name) const;
    void                 set_position(const position&);
    position             get_position() const;
    void                 set_size(const size&);
    size                 get_size() const;
    std::optional<rect>  get_border_size() const;
    void                 set_minimal_size(const size&);
    size                 get_minimal_size() const;
    void                 set_maximum_size(const size&);
    size                 get_maximum_size() const;
    void                 set_bordered(bool);
    bool                 bordered() const;
    void                 set_resizable(bool);
    bool                 resizeable() const;
    void                 show();
    bool                 shown() const;
    void                 hide();
    bool                 hidden() const;
    void                 raise();
    void                 maximize();
    bool                 maximized() const;
    void                 minimize();
    bool                 minimized() const;
    void                 restore();
    void                 set_fullscreen(bool);
    bool                 fullscreen() const;
    void                 set_fullscreen_desktop(bool);
    bool                 fullscreen_desktop() const;
    surface              get_surface() const;
    bool                 update_surface(const surface&);
    bool                 update_surface_rects(const std::vector<rect>& rects);
    void                 grab_input(bool);
    bool                 input_grabbed() const;
    bool                 set_brightness(const float&);
    float                get_brightness() const;
    bool                 set_opacity(const float&);
    std::optional<float> get_opacity() const;
    bool                 set_modal_for(window& parent);
    void                 set_input_focus();
    bool                 has_input_focus() const;
    bool                 has_mouse_focus() const;
    bool                 has_mouse_captured() const;
    bool                 is_always_ontop() const;
    bool                 set_gamma_ramp(
                        const std::optional<std::array<std::uint16_t, 256>*> red,
                        const std::optional<std::array<std::uint16_t, 256>*> green,
                        const std::optional<std::array<std::uint16_t, 256>*> blue);
    bool get_gamma_ramp(std::optional<std::array<std::uint16_t, 256>*> red,
                        std::optional<std::array<std::uint16_t, 256>*> green,
                        std::optional<std::array<std::uint16_t, 256>*> blue);
    // bool set_hit_test(hit_test callback, std::any);
    void close();
    virtual ~window();

private:
    window(const char* title, size window_size, position window_position,
           mode window_mode);
    bool valid = false;
    class impl;
    std::unique_ptr<impl> data;

    friend class video;
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

class gl_context
{
public:
    gl_context(const gl_context&) = delete;
    gl_context& operator=(const gl_context&) = delete;
    gl_context& operator=(gl_context&&) = delete;

    gl_context(gl_context&&);
    ~gl_context();

private:
    gl_context();
    class impl;
    impl* data = nullptr;

    friend class video;
};

enum class gl_swap_interval
{
    immediate,
    synchronized,
    late_swaps
};

struct video
{
    static std::vector<std::string_view> get_drivers();
    void                                 init(const char* driver_name);
    void                                 quit();

    window     create_window(const char* title, const window::size window_size,
                             std::optional<window::position> window_position,
                             std::optional<window::mode>     window_mode);
    gl_context gl_create_context(const window&);

    std::string_view      get_current_driver() const;
    std::vector<display>  get_displays() const;
    display               get_display_for_window(const window&) const;
    window                create_window_from(const void* native_handle);
    window                get_window_fromID(std::uint32_t id) const;
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
    bool       gl_make_current(const window& window, const gl_context& context);
    window     gl_get_current_window();
    gl_context gl_get_current_context();
    window::size gl_get_drawable_size(const window&);
    bool         gl_set_swap_interval(gl_swap_interval);
    void         gl_swap_window(const window&);
    void         gl_delete_context(const gl_context&);
};

struct sdlxx
{
    explicit sdlxx();
    ~sdlxx();
    sdlxx(const sdlxx&) = delete;
    sdlxx& operator=(const sdlxx&) = delete;

}; // end sdlxx
} // end namespace om
