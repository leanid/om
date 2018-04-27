#include "engine_impl.hxx"
#include "om/game.hxx"

#include <cstdlib>
//#if __has_include(<filesystem>)
//#include <filesystem>
// namespace fs = std::filesystem;
//#elif __has_include("experimental/filesystem")
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
//#endif
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include <SDL2/SDL_loadso.h>

namespace om
{
struct event
{
};

game::~game()
{
}
}

void             init_minimal_log_system();
void             start_game(om::engine_impl&);
bool             pool_event(om::event&);
std::string_view get_cxx_mangled_name();

int main(int argc, char* argv[])
{
    try
    {
        init_minimal_log_system();

        om::engine_impl engine(argc, argv);

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

#if defined(OM_STATIC)
std::unique_ptr<om::game> call_create_game(om::engine& e)
{
    std::unique_ptr<om::game> game = create_game(e);
    return game;
}
#endif

#if !defined(OM_STATIC)

#if defined(__MINGW32__) || defined(__linux__)
std::string_view get_cxx_mangled_name()
{
    return "_Z11create_gameRN2om6engineE";
}
#elif defined(_MSC_VER)
std::string_view get_cxx_mangled_name()
{
    return "create_game::om::engine";
}
#else
#error "add mangled name for your compiler"
#endif

#if defined(_WIN32) && defined(__MINGW32__)
std::string get_game_library_path(om::engine&)
{
    return "libgame.dll";
}
#elif defined(_WIN32) && defined(_MSC_VER)
std::string get_game_library_path(om::engine&)
{
    return "game.dll";
}
#elif defined(__linux__)
std::string get_game_library_path(om::engine&)
{
    return "./libgame.so";
}
#else
std::string get_game_library_path(om::engine&)
{
#error implement it
}
#endif

std::unique_ptr<om::game> call_create_game(om::engine_impl& e)
{
    using namespace std::string_literals;

    auto game_so_name = get_game_library_path(e);
    auto tmp_game_so  = "tmp_" + game_so_name;

    {
        std::ifstream src_so(game_so_name, std::ios::binary);
        std::ofstream dst_so(tmp_game_so, std::ios::binary);
        dst_so << src_so.rdbuf();
    }

    void* so_handle = SDL_LoadObject(tmp_game_so.c_str());
    if (nullptr == so_handle)
    {
        throw std::runtime_error("can't load: "s + tmp_game_so);
    }

    e.so_handle = so_handle;

    std::string_view func_name = get_cxx_mangled_name();

    void* func_addres = SDL_LoadFunction(so_handle, func_name.data());

    if (nullptr == func_addres)
    {
        throw std::runtime_error(
            "can't find "s +
            "std::unique_ptr<om::game> create_game(om::engine&) "s +
            "mangled name: "s + func_name.data() + " in so: " + tmp_game_so);
    }

    std::unique_ptr<om::game> (*func_ptr)(om::engine&);
    func_ptr = reinterpret_cast<std::unique_ptr<om::game> (*)(om::engine&)>(
        func_addres);

    std::unique_ptr<om::game> game = func_ptr(e);
    return game;
}
#endif

void start_game(om::engine_impl& e)
{
    std::unique_ptr<om::game> game = call_create_game(e);

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

    auto proccess_events = [&game]() {
        om::event event;
        while (pool_event(event))
        {
            game->proccess_input(event);
        }
    };

    fs::path           path       = get_game_library_path(e);
    fs::file_time_type last_write = fs::last_write_time(path);

    std::time_t cftime = fs::file_time_type::clock::to_time_t(last_write);
    std::cout << std::asctime(std::localtime(&cftime)) << std::endl;

    double timeout_reload_game  = 0.0; // seconds
    bool   reload_timer_started = false;

    while (!game->is_closed())
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

        fs::file_time_type last_time = fs::last_write_time(path);
        if (last_time != last_write)
        {
            reload_timer_started = true;
            timeout_reload_game  = 2.0; // second
            last_write           = last_time;
        }

        if (reload_timer_started)
        {
            timeout_reload_game -= frame_delta.count() * 0.001;
            if (timeout_reload_game >= 0)
            {
                std::cout << "reloading library!" << std::endl;

                reload_timer_started = false;

                game.reset();
                SDL_UnloadObject(e.so_handle);

                game = call_create_game(e);
            }
        }

        start = end_last_frame;
    }
}
