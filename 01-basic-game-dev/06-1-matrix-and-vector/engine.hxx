#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

#ifndef OM_DECLSPEC
#define OM_DECLSPEC
#endif

namespace om
{

struct OM_DECLSPEC vec2
{
    vec2();
    vec2(float x, float y);
    float x = 0;
    float y = 0;
};

OM_DECLSPEC vec2 operator+(const vec2& l, const vec2& r);

struct OM_DECLSPEC mat2
{
    mat2();
    static mat2 identity();
    static mat2 scale(float scale);
    static mat2 rotation(float thetha);
    vec2        col0;
    vec2        col1;
};

OM_DECLSPEC vec2 operator*(const vec2& v, const mat2& m);
OM_DECLSPEC mat2 operator*(const mat2& m1, const mat2& m2);

/// dendy gamepad emulation events
enum class event
{
    /// input events
    left_pressed,
    left_released,
    right_pressed,
    right_released,
    up_pressed,
    up_released,
    down_pressed,
    down_released,
    select_pressed,
    select_released,
    start_pressed,
    start_released,
    button1_pressed,
    button1_released,
    button2_pressed,
    button2_released,
    /// virtual console events
    turn_off
};

OM_DECLSPEC std::ostream& operator<<(std::ostream& stream, const event e);

class engine;

/// return not null on success
OM_DECLSPEC engine* create_engine();
OM_DECLSPEC void    destroy_engine(engine* e);

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

/// vertex with position only
struct OM_DECLSPEC v0
{
    vec2 pos;
};

/// vertex with position and texture coordinate
struct OM_DECLSPEC v1
{
    vec2  pos;
    color c;
};

/// vertex position + color + texture coordinate
struct OM_DECLSPEC v2
{
    vec2  pos;
    vec2  uv;
    color c;
};

/// triangle with positions only
struct OM_DECLSPEC tri0
{
    tri0();
    v0 v[3];
};

/// triangle with positions and color
struct OM_DECLSPEC tri1
{
    tri1();
    v1 v[3];
};

/// triangle with positions color and texture coordinate
struct OM_DECLSPEC tri2
{
    tri2();
    v2 v[3];
};

OM_DECLSPEC std::istream& operator>>(std::istream& is, mat2&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, vec2&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, color&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, v0&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, v1&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, v2&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, tri0&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, tri1&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, tri2&);

class OM_DECLSPEC texture
{
public:
    virtual ~texture();
    virtual std::uint32_t get_width() const  = 0;
    virtual std::uint32_t get_height() const = 0;
};

class OM_DECLSPEC engine
{
public:
    virtual ~engine();
    /// create main window
    /// on success return empty string
    virtual std::string initialize(std::string_view config) = 0;
    /// return seconds from initialization
    virtual float get_time_from_init() = 0;
    /// pool event from input queue
    /// return true if more events in queue
    virtual bool     read_input(event& e)                  = 0;
    virtual texture* create_texture(std::string_view path) = 0;
    virtual void     destroy_texture(texture* t)           = 0;
    virtual void     render(const tri0&, const color&)     = 0;
    virtual void     render(const tri1&)                   = 0;
    virtual void     render(const tri2&, texture*)         = 0;
    virtual void     swap_buffers()                        = 0;
    virtual void     uninitialize()                        = 0;
};

} // end namespace om
