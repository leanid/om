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

std::ostream& OM_DECLSPEC operator<<(std::ostream& stream, const event e);

class engine;

/// return not null on success
engine* OM_DECLSPEC create_engine();
void OM_DECLSPEC destroy_engine(engine* e);

struct vertex
{
    vertex()
        : x(0.f)
        , y(0.f)
    {
    }
    float x;
    float y;
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

std::istream& OM_DECLSPEC operator>>(std::istream& is, vertex&);
std::istream& OM_DECLSPEC operator>>(std::istream& is, triangle&);

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
