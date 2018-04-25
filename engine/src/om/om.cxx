#include "om/om.hxx"

#include <cstdlib>
#include <iostream>

namespace om
{
struct engine
{
};
}

void init_minimal_log_system();
void start_game(om::engine&);
bool pool_event(om::event&);

int main(int argc, char* argv[])
{
    try
    {
        init_minimal_log_system();

        om::engine engine(argc, argv);

        start_game(engine);

        return EXIT_SUCCESS;
    }
    catch (std::exception& ex)
    {
        std::cerr << "error: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "error: unknown exception" << std::endl;
    }
    return EXIT_FAILURE;
}

void init_minimal_log_system()
{
    // TODO on android redirect std::cout to adb logcat
}

bool pool_event(om::event&)
{
    return false;
}

void proccess_events()
{
    om::event event;
    while (pool_event(event))
    {
        game->proccess_event(event);
    }
}

void start_game(om::engine& e)
{
    std::unique_ptr<om::igame> game = create_game(e);

    if (!game)
    {
        return;
    }

    namespace time    = std::chrono;
    using clock_timer = time::high_resolution_clock;
    using nano_sec    = time::nanoseconds;
    using time_point  = time::time_point<clock_timer, nano_sec>;

    clock_timer timer;

    time_point start = timer.now();

    game->initialize();

    while (!game->closed())
    {
        time_point end_last_frame = timer.now();

        auto frame_delta =
            time::duration_cast<om::milliseconds>(end_last_frame - start);

        if (frame_delta.count() < 15) // 1000 % 60(FPS) = 16.666 ms
        {
            std::this_thread::yield(); // too fast, give other apps CPU time
            continue;                  // wait till more time
        }

        proccess_events();

        game->update(frame_delta);
        game->draw();

        start = end_last_frame;
    }
}
