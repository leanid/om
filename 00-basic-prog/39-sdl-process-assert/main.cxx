#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "show_message.hxx"

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

int main(int argc, char** argv)
{
#if !__cpp_lib_span_initializer_list
#error "need __cpp_lib_span_initializer_list"
#endif

    namespace po = boost::program_options;

    po::options_description message_options("message options");
    message_options.add_options()("help", "print usage");
    message_options.add_options()(
        "pipe", "use stdin/stdout/stderr pipe to communicate");
    message_options.add_options()("button",
                                  po::value<std::vector<std::string>>(),
                                  "multiple buttons names, order is important");
    message_options.add_options()(
        "title", po::value<std::string>(), "title utf8 string value");
    message_options.add_options()(
        "text", po::value<std::string>(), "text utf8 message");

    po::command_line_parser parser{ argc, argv };
    parser.options(message_options);
    po::parsed_options parsed_options = parser.run();

    po::variables_map vm;
    po::store(parsed_options, vm);

    if (vm.count("help"))
    {
        std::cout << message_options << std::endl;
        return EXIT_SUCCESS;
    }

    std::vector<std::u8string> buttons_in;
    if (vm.count("button"))
    {
        auto values = vm["button"].as<std::vector<std::string>>();
        for (auto& v : values)
        {
            std::cout << v << std::endl;
            buttons_in.emplace_back();
            buttons_in.back().append_range(v);
            std::cout << std::string(buttons_in.back().begin(),
                                     buttons_in.back().end())
                      << std::endl;
        }
    }

    std::u8string text;
    if (vm.count("text"))
    {
        auto str = vm["text"].as<std::string>();
        text.append_range(str);
    }

    std::u8string title;
    if (vm.count("title"))
    {
        auto str = vm["title"].as<std::string>();
        title.append_range(str);
    }

    if (!text.empty())
    {
        auto        index = om::gui::show_message(title, text, buttons_in);
        auto        str   = buttons_in.at(index);
        std::string str_ascii(str.begin(), str.end());
        std::cout << "your selection is: " << str_ascii << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("pipe"))
    {
        std::cout << "start pipe" << std::endl;
        om::gui::msg_box message;
        std::string      command;
        for (std::cin >> command; !command.empty(); std::cin >> command)
        {
            if (command == "button")
            {
                std::string button;
                std::cin >> button;
                std::cout << "add button: " << button << std::endl;
                message.add_button(std::u8string(button.begin(), button.end()));
            }
            else if (command == "text")
            {
                std::string line;
                std::getline(std::cin, line);
                std::cout << "add text: " << line << std::endl;
                message.text(std::u8string(line.begin(), line.end()));
            }
            command.clear();
        }
        uint32_t selected_button = message.show();
        std::cout << selected_button << std::endl;
        return EXIT_SUCCESS;
    }

    std::array<std::u8string, 4> buttons = {
        u8"да", u8"отмена", u8"далее", u8"стоп"
    };

    uint32_t result = om::gui::show_message(
        u8"заголовок сообщения", u8"произвольный текст для примера", buttons);

    auto& button_text = buttons.at(result);
    std::cout << "your selection is: "
              << reinterpret_cast<const char*>(button_text.data()) << '\n';
    return EXIT_SUCCESS;
}
