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

OM_DECLSPEC vec2 operator+(const vec2& l, const vec2& r);

struct OM_DECLSPEC mat2x3
{
    mat2x3();
    static mat2x3 identiry();
    static mat2x3 scale(float scale);
    static mat2x3 scale(float sx, float sy);
    static mat2x3 rotation(float thetha);
    static mat2x3 move(const vec2& delta);
    vec2          col0;
    vec2          col1;
    vec2          delta;
};

OM_DECLSPEC vec2 operator*(const vec2& v, const mat2x3& m);
OM_DECLSPEC mat2x3 operator*(const mat2x3& m1, const mat2x3& m2);

/// dendy gamepad emulation events
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

OM_DECLSPEC std::ostream& operator<<(std::ostream& stream, const input_data&);
OM_DECLSPEC std::ostream& operator<<(std::ostream& stream,
                                     const hardware_data&);
OM_DECLSPEC std::ostream& operator<<(std::ostream& stream, const event& e);

OM_DECLSPEC std::istream& operator>>(std::istream& is, mat2x3&);
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

class OM_DECLSPEC vbo
{
public:
    virtual ~vbo();
    virtual const v2* data() const = 0;
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

class OM_DECLSPEC engine
{
public:
    explicit engine(std::string_view config);
    ~engine();
    engine& operator      =(engine&& other); // move assignment
    engine(const engine&) = delete;

    /// return seconds from initialization
    float get_time_from_init();
    /// pool event from input queue
    bool read_event(event& e);
    bool is_key_down(const enum keys);

    texture* create_texture(std::string_view path);
    void     destroy_texture(texture* t);

    vbo* create_vbo(const tri2*, std::size_t);
    void destroy_vbo(vbo*);

    sound* create_sound(std::string_view path);
    void   destroy_sound(sound*);

    void render(const tri0&, const color&);
    void render(const tri1&);
    void render(const tri2&, texture*);
    void render(const tri2&, texture*, const mat2x3& m);
    void render(const vbo&, texture*, const mat2x3&);
    void swap_buffers();
    void uninitialize();

    void exit(int return_code);

    std::ostream& log;
};

struct OM_DECLSPEC lila
{
    virtual ~lila();
    virtual void on_initialize()                                  = 0;
    virtual void on_event(om::event&)                             = 0;
    virtual void on_update(std::chrono::milliseconds frame_delta) = 0;
    virtual void on_render() const                                = 0;
};

} // end namespace om

extern std::unique_ptr<om::lila> om_tat_sat(om::engine&);
