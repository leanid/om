#include <chrono>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

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

vec2 OM_DECLSPEC operator+(const vec2& l, const vec2& r);

struct OM_DECLSPEC matrix
{
    matrix();
    static matrix identiry();
    static matrix scale(float scale);
    static matrix scale(float sx, float sy);
    static matrix rotation(float thetha);
    static matrix move(const vec2& delta);
    vec2 row0;
    vec2 row1;
    vec2 row2;
};

vec2 OM_DECLSPEC operator*(const vec2& v, const matrix& m);
matrix OM_DECLSPEC operator*(const matrix& m1, const matrix& m2);

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
    double     timestamp;
    event_type type;
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

std::ostream& OM_DECLSPEC operator<<(std::ostream& stream, const input_data&);
std::ostream& OM_DECLSPEC operator<<(std::ostream& stream,
                                     const hardware_data&);
std::ostream& OM_DECLSPEC operator<<(std::ostream& stream, const event e);

std::istream& OM_DECLSPEC operator>>(std::istream& is, matrix&);
std::istream& OM_DECLSPEC operator>>(std::istream& is, vec2&);
std::istream& OM_DECLSPEC operator>>(std::istream& is, color&);
std::istream& OM_DECLSPEC operator>>(std::istream& is, vertex&);

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

/// return seconds from initialization
float OM_DECLSPEC get_time_from_init();

bool OM_DECLSPEC pool_event(event& e);

bool OM_DECLSPEC is_key_down(const enum keys);

texture* OM_DECLSPEC create_texture(std::string_view path);
void OM_DECLSPEC destroy_texture(texture* t);

vbo* OM_DECLSPEC create_vbo(const vertex*, std::size_t);
void OM_DECLSPEC destroy_vbo(vbo*);

sound* OM_DECLSPEC create_sound(std::string_view path);
void OM_DECLSPEC destroy_sound(sound*);

enum class primitives
{
    lines,
    line_strip,
    line_loop,
    triangls,
    trianglestrip,
    trianglfan
};

void OM_DECLSPEC render(const enum primitives, const vbo&, const texture*,
                        const matrix&);

void OM_DECLSPEC exit(int return_code);

extern OM_DECLSPEC std::ostream& log;

struct OM_DECLSPEC lila
{
    virtual ~lila();
    virtual void on_initialize()                                  = 0;
    virtual void on_event(om::event&)                             = 0;
    virtual void on_update(std::chrono::milliseconds frame_delta) = 0;
    virtual void on_render() const                                = 0;
};

} // end namespace om

extern std::unique_ptr<om::lila> om_tat_sat();
