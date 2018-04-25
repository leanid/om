#pragma once

#include <chrono>
#include <memory>

namespace om
{
struct event;
struct engine;

using milliseconds = std::chrono::milliseconds;

struct game
{
    virtual void initialize()                     = 0;
    virtual void proccess_input(event& e)         = 0;
    virtual void update(milliseconds frame_delta) = 0;
    virtual void draw() const                     = 0;
    virtual bool is_closed() const                = 0;

    virtual ~game();
};

} // end namespace om

extern std::unique_ptr<om::game> create_game(om::engine&);
