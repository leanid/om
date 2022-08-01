#pragma once

#include <chrono>
#include <memory>

#if !defined(OM_EXP)
#if defined(_WIN32)
#define OM_EXP __declspec(dllimport)
#else
#define OM_EXP
#endif
#endif

#if !defined(OM_GAME)
#define OM_GAME
#endif

namespace om
{
struct event;
struct engine;

using milliseconds = std::chrono::milliseconds;

struct OM_EXP game
{
    virtual void               initialize()                     = 0;
    virtual void               process_input(event& e)         = 0;
    virtual void               update(milliseconds frame_delta) = 0;
    virtual void               draw() const                     = 0;
    [[nodiscard]] virtual bool is_closed() const                = 0;

    virtual ~game();
};

} // end namespace om

extern std::unique_ptr<om::game> OM_GAME create_game(om::engine&);
