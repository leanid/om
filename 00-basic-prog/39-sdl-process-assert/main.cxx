#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "show_message.hxx"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
#if !__cpp_lib_span_initializer_list
#error "need __cpp_lib_span_initializer_list"
#endif

    std::array<std::u8string_view, 4> buttons = {
        u8"ok", u8"cansel", u8"next", u8"break"
    };

    auto result = om::show_message(
        u8"заголовок сообщения", u8"произвольный текст для примера", buttons);

    if (!result.has_value())
    {
        std::cerr << "error: " << result.error().message() << '\n';
        return EXIT_FAILURE;
    }
    auto& button_text = buttons.at(result.value());
    std::cout << "your select: "
              << reinterpret_cast<const char*>(button_text.data()) << '\n';
    return EXIT_SUCCESS;
}
