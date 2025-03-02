#include "show_message.hxx"

#include "SDL3/SDL.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

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
    std::filesystem::path      base        = SDL_GetBasePath();
    std::filesystem::path      binary_name = base / "39-sdl-process-assert";
    std::array<const char*, 3> args        = { binary_name.c_str(),
                                               "--pipe",
                                               nullptr };
    std::unique_ptr<SDL_Process, void (*)(SDL_Process*)> child{
        SDL_CreateProcess(args.data(), true), SDL_DestroyProcess
    };

    if (!child)
    {
        throw std::runtime_error(SDL_GetError());
    }
    std::cout << "process created" << std::endl;
    SDL_IOStream* child_input = SDL_GetProcessInput(child.get());

    std::ostringstream ss;
    ss << *this;
    std::string data    = ss.str();
    size_t      written = SDL_WriteIO(child_input, data.data(), data.size());
    if (written != data.size())
    {
        throw std::runtime_error("can't write data to child stdin");
    }

    if (!SDL_CloseIO(child_input))
    {
        throw std::runtime_error("can't close child input");
    }

    SDL_IOStream* child_output = SDL_GetProcessOutput(child.get());

    std::stringstream iss;
    std::string       tmp(1024u, '\0');
    for (size_t num = SDL_ReadIO(child_output, tmp.data(), tmp.size()); num > 0;
         num        = SDL_ReadIO(child_output, tmp.data(), tmp.size()))
    {
        iss << tmp.substr(num);
    }

    std::cout << "subprocess stdout: " << iss.str() << std::endl;

    int exitcode{};
    std::ignore = SDL_WaitProcess(child.get(), true, &exitcode);
    std::cout << "subprocess exitcode: " << exitcode << std::endl;

    int32_t user_select = -1;
    iss >> user_select;

    std::cout << "user select button index: " << user_select << '\n';
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
