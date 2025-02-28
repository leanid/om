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
        u8"да", u8"отмена", u8"далее", u8"стоп"
    };

    uint32_t result = om::show_message(
        u8"заголовок сообщения", u8"произвольный текст для примера", buttons);

    auto& button_text = buttons.at(result);
    std::cout << "your selection is: "
              << reinterpret_cast<const char*>(button_text.data()) << '\n';
    return EXIT_SUCCESS;
}
