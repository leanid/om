#pragma once

#include <chrono>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <iostream>

#ifndef OM_DECLSPEC
#define OM_DECLSPEC
#endif

#include "math.hxx"

namespace om
{

/// Dendy gamepad emulation events
enum class event_type
{
    input_key,
    hardware
};

enum class keys
{
    left,
    right,
    up,
    down,
    select,
    start,
    button1,
    button2
};

struct input_data
{
    enum keys key;
    bool      is_down;
};

struct hardware_data
{
    bool is_reset;
};

struct event
{
    std::variant<input_data, hardware_data> info;
    double                                  timestamp;
    event_type                              type;
};

class OM_DECLSPEC color
{
public:
    color() = default;
    explicit color(std::uint32_t rgba_);
    color(float r, float g, float b, float a);

    float get_r() const;
    float get_g() const;
    float get_b() const;
    float get_a() const;

    void set_r(const float r);
    void set_g(const float g);
    void set_b(const float b);
    void set_a(const float a);

private:
    std::uint32_t rgba = 0;
};

/// vertex position + color + texture coordinate
struct OM_DECLSPEC vertex
{
    vec2  pos;
    vec2  uv;
    color c;
};

OM_DECLSPEC std::ostream& operator<<(std::ostream& stream, const input_data&);
OM_DECLSPEC std::ostream& operator<<(std::ostream& stream,
                                     const hardware_data&);
OM_DECLSPEC std::ostream& operator<<(std::ostream& stream, const event& e);

OM_DECLSPEC std::istream& operator>>(std::istream& is, matrix&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, vec2&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, color&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, vertex&);

class OM_DECLSPEC texture
{
public:
    virtual ~texture();
    virtual std::uint32_t get_width() const  = 0;
    virtual std::uint32_t get_height() const = 0;
};

class OM_DECLSPEC vbo
{
public:
    virtual ~vbo();
    virtual const vertex* data() const = 0;
    /// count of vertexes
    virtual size_t size() const = 0;
};

class OM_DECLSPEC sound
{
public:
    enum class effect
    {
        once,
        looped
    };

    virtual ~sound();
    virtual void play(const effect) = 0;
    virtual bool is_playing() const = 0;
    virtual void stop()             = 0;
};

struct window_mode
{
    size_t width         = 640;
    size_t heigth        = 480;
    bool   is_fullscreen = false;
};

OM_DECLSPEC void initialize(std::string_view   title,
                            const window_mode& desired_window_mode);

OM_DECLSPEC window_mode get_current_window_mode();

/// return seconds from initialization
OM_DECLSPEC float get_time_from_init();

OM_DECLSPEC bool pool_event(event& e);

OM_DECLSPEC bool is_key_down(const enum keys);

OM_DECLSPEC texture* create_texture(std::string_view path);
OM_DECLSPEC void     destroy_texture(texture* t);

OM_DECLSPEC vbo* create_vbo(const vertex*, std::size_t);
OM_DECLSPEC void destroy_vbo(vbo*);

OM_DECLSPEC sound* create_sound(std::string_view path);
OM_DECLSPEC void   destroy_sound(sound*);

OM_DECLSPEC void get_window_size(size_t& width, size_t& height);

enum class primitives
{
    lines,
    line_strip,
    line_loop,
    triangls,
    trianglestrip,
    trianglfan
};

OM_DECLSPEC void render(const primitives, const vbo&, const texture*,
                        const matrix&);

OM_DECLSPEC void exit(int return_code);

extern OM_DECLSPEC std::ostream& log;

struct OM_DECLSPEC lila
{
    virtual ~lila();
    virtual void on_initialize()                                  = 0;
    virtual void on_event(om::event&)                             = 0;
    virtual void on_update(std::chrono::milliseconds frame_delta) = 0;
    virtual void on_render() const                                = 0;
};

    struct OM_DECLSPEC membuf : public std::streambuf
    {
        membuf()
                : std::streambuf()
                , buf()
                , buf_size(0)
        {
        }
        membuf(std::unique_ptr<char[]> buffer, size_t size)
                : std::streambuf()
                , buf(std::move(buffer))
                , buf_size(size)
        {
            char* beg_ptr = buf.get();
            char* end_ptr = beg_ptr + buf_size;
            setg(beg_ptr, beg_ptr, end_ptr);
            setp(beg_ptr, end_ptr);
        }
        membuf(membuf&& other)
        {
            setp(nullptr, nullptr);
            setg(nullptr, nullptr, nullptr);

            other.swap(*this);

            buf      = std::move(other.buf);
            buf_size = other.buf_size;

            other.buf_size = 0;
        }

        pos_type seekoff(off_type pos, std::ios_base::seekdir seek_dir,
                         std::ios_base::openmode) override
        {
            // TODO implement it in correct way
            if (seek_dir == std::ios_base::beg)
            {
                return 0 + pos;
            }
            else if (seek_dir == std::ios_base::end)
            {
                return buf_size + pos;
            }
            else
            {
                return egptr() - gptr();
            }
        }

        char*  begin() const { return eback(); }
        size_t size() const { return buf_size; }

    private:
        std::unique_ptr<char[]> buf;
        size_t                  buf_size;
    };


    OM_DECLSPEC membuf load_file(std::string_view path);

} // end namespace om

extern std::unique_ptr<om::lila> om_tat_sat();
