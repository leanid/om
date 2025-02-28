#include "show_message.hxx"

#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"

#include <stdexcept>
#include <vector>

namespace om
{
uint32_t show_message(std::u8string_view            title,
                      std::u8string_view            text,
                      std::span<std::u8string_view> button_names)
{
    std::vector<SDL_MessageBoxButtonData> button_data;
    button_data.reserve(button_names.size());

    for (size_t i = 0; i < button_names.size(); ++i)
    {
        SDL_MessageBoxButtonData button;
        button.flags    = 0;
        button.buttonID = static_cast<int>(i);
        button.text     = reinterpret_cast<const char*>(button_names[i].data());
        button_data.push_back(button);
    }

    SDL_MessageBoxData messageboxdata;
    messageboxdata.flags       = SDL_MESSAGEBOX_INFORMATION;
    messageboxdata.window      = nullptr;
    messageboxdata.title       = reinterpret_cast<const char*>(title.data());
    messageboxdata.message     = reinterpret_cast<const char*>(text.data());
    messageboxdata.numbuttons  = static_cast<int>(button_data.size());
    messageboxdata.buttons     = button_data.data();
    messageboxdata.colorScheme = nullptr;

    int buttonid;
    if (!SDL_ShowMessageBox(&messageboxdata, &buttonid))
    {
        using namespace std::string_literals;
        std::string error = "error: displaying message box: "s + SDL_GetError();
        throw std::runtime_error(error);
    }

    return buttonid;
}

} // namespace om
