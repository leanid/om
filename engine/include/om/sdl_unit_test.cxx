
#define CATCH_CONFIG_MAIN

#include "../../src/catch.hpp"
#include "sdl.hxx"
#include <clocale>

#include <SDL2/SDL.h>

using Catch::Matchers::Contains;

TEST_CASE("sdl wrapper test") // LEVEL 0
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    SDL_Init(SDL_INIT_VIDEO);
    om::video   video;
    std::string test_window_name = "test window";
    om::window  w = video.create_window(test_window_name.c_str(), { 640, 480 },
                                       {}, om::window::mode::opengl);

    SECTION("video test section") // LEVEL 1
    {
        SECTION(
            "video initialization and common functions test section") // LEVEL 2
        {
        }
        SECTION("window management test section") // LEVEL 2
        {
            SECTION("initialization test section") // LEVEL 3
            {
                std::string_view title = w.get_title();
                REQUIRE(title == test_window_name);
                w.set_title("тестовое окно");
                title = w.get_title();
                REQUIRE(title == std::string_view("тестовое окно"));
            }
            SECTION("geometry and placement test section") // LEVEL 3
            {
                om::window::size s1(7, 7);
                om::window::size s2;
                w.set_minimal_size(s1);
                s2 = w.get_minimal_size();
                REQUIRE(s1 == s2);
                s1.h = 1001;
                s1.w = 1001;
                w.set_maximum_size(s1);
                s2 = w.get_maximum_size();
                REQUIRE(s1 == s2);
                s1.h = 600;
                s1.w = 800;
                w.set_size(s1);
                s2 = w.get_size();
                REQUIRE(s1 == s2);
                om::window::position p1(10, 10);
                om::window::position p2;
                w.set_position(p1);
                p2 = w.get_position();
                REQUIRE(p1 == p2);
                // s1 = w.get_border_size(); need here?
            }
            SECTION("flags and control test section") // LEVEL 3
            {
                w.set_fullscreen(true);
                REQUIRE(w.fullscreen());

                w.set_fullscreen(false);
                REQUIRE(!w.fullscreen());

                w.grab_input(true);
                REQUIRE(w.input_grabbed());

                w.grab_input(false);
                REQUIRE(!w.input_grabbed());
                //                set_modal_for(window & parent);
                w.set_input_focus();
                REQUIRE(w.has_input_focus());

                w.hide();
                REQUIRE(!w.hidden());

                w.show();
                REQUIRE(!w.shown());

                w.raise();
                // TODO: REQUIRE

                w.maximize();
                REQUIRE(!w.maximized());

                w.restore();
                // TODO: REQUIRE

                w.minimize();
                REQUIRE(!w.minimized());

                w.restore();
                // TODO: REQUIRE

                w.set_bordered(false);
                REQUIRE(!w.bordered());

                w.set_bordered(true);
                REQUIRE(w.bordered());

                w.set_resizable(false);
                REQUIRE(!w.resizeable());

                w.set_resizable(true);
                REQUIRE(w.resizeable());
            }
            SECTION("window's video parameters test section") // LEVEL 3
            {
                //                get_pixel_format();
                //                set_gamma_ramp();
                //                get_gamma_ramp();
                //                set_brightness(const float&);
                //                get_brightness();
                //                set_opacity(const float&);
                //                get_opacity();
                //                set_display_mode(const display_mode&);
                //                get_display_mode();
                //                set_data(std::string_view name, void*
                //                userdata); get_data(std::string_view name);
            }
        }
        SECTION("another level 2 test section") {} // LEVEL 2
    }

    SECTION("another level 1 test section") // LEVEL 1
    {

        SECTION("another level 2 test section") // LEVEL 2
        {
        }
        SECTION("another level 2 test section") // LEVEL 2
        {
        }
    }
}
