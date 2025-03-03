#include "show_message.hxx"

#include "SDL3/SDL.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace om::gui
{

uint32_t show_message(std::string            title,
                      std::string            text,
                      std::span<std::string> button_names)
{
    std::vector<SDL_MessageBoxButtonData> button_data;
    button_data.reserve(button_names.size());

    for (size_t i = 0; i < button_names.size(); ++i)
    {
        SDL_MessageBoxButtonData button;
        button.flags    = 0;
        button.buttonID = static_cast<int>(i);
        button.text = reinterpret_cast<const char*>(button_names[i].c_str());
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

uint32_t msg_box::show()
{
    std::vector<std::string> button_names;
    std::ranges::transform(buttons_,
                           std::back_inserter(button_names),
                           [](auto& btn) { return btn.name; });
    return gui::show_message(title_, text_, button_names);
}

uint32_t msg_box::show_in_child_process()
{
    std::cout << __FUNCTION__ << std::endl;
    std::filesystem::path      base          = SDL_GetBasePath();
    std::filesystem::path      binary_name   = base / "39-sdl-process-assert";
    std::u8string              u8binary_name = binary_name.u8string();
    std::array<const char*, 3> args          = {
        reinterpret_cast<const char*>(u8binary_name.c_str()), "--pipe", nullptr
    };
    std::unique_ptr<SDL_Process, void (*)(SDL_Process*)> child{
        SDL_CreateProcess(args.data(), true), SDL_DestroyProcess
    };

    if (!child)
    {
        const char* error_msg = SDL_GetError();
        throw std::runtime_error(error_msg);
    }
    std::cout << "process created" << std::endl;

    {
        std::unique_ptr<SDL_IOStream, bool (*)(SDL_IOStream*)> child_input{
            SDL_GetProcessInput(child.get()), SDL_CloseIO
        };

        std::ostringstream ss;
        ss << *this;
        std::string data = ss.str();
        size_t      written =
            SDL_WriteIO(child_input.get(), data.data(), data.size());
        if (written != data.size())
        {
            throw std::runtime_error("can't write data to child stdin");
        }
    }

    size_t                                 datasize{};
    int                                    exitcode{};
    std::unique_ptr<void, void (*)(void*)> ptr{
        SDL_ReadProcess(child.get(), &datasize, &exitcode), SDL_free
    };
    std::string_view output(static_cast<char*>(ptr.get()), datasize);

    std::cout << "subprocess stdout: " << output << std::endl;

    std::cout << "subprocess exitcode: " << exitcode << std::endl;

    return 0;
}

std::ostream& operator<<(std::ostream& out, const msg_box& msg)
{
    out << "title " << msg.title_ << '\n';
    for (auto& button : msg.buttons_)
    {
        out << "button " << button.name << '\n';
    }
    out << "text " << msg.text_ << '\n';
    return out;
}
} // namespace om::gui
