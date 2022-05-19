#include <iosfwd>
#include <string>
#include <string_view>

#ifndef OM_DECLSPEC
#define OM_DECLSPEC
#endif

namespace om
{
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

struct OM_DECLSPEC vertex
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct OM_DECLSPEC triangle
{
    triangle()
    {
        v[0] = vertex();
        v[1] = vertex();
        v[2] = vertex();
    }
    vertex v[3];
};

OM_DECLSPEC std::istream& operator>>(std::istream& is, vertex&);
OM_DECLSPEC std::istream& operator>>(std::istream& is, triangle&);

class OM_DECLSPEC engine
{
public:
    virtual ~engine();
    /// create main window
    /// on success return empty string
    virtual std::string initialize(std::string_view config) = 0;
    /// pool event from input queue
    /// return true if more events in queue
    virtual bool read_input(event& e)             = 0;
    virtual void render_triangle(const triangle&) = 0;
    virtual void swap_buffers()                   = 0;
    virtual void uninitialize()                   = 0;
};

} // end namespace om
