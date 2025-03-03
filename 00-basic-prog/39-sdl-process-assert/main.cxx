#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "show_message.hxx"

#include <boost/program_options.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

namespace om
{
om::gui::msg_box parse_args_or_stdin(int argc, char** argv);
}

int main(int argc, char** argv)
{
    //try
    {

        om::gui::msg_box msg_box = om::parse_args_or_stdin(argc, argv);

#if 1
        std::cout << "after parsing: " << msg_box << std::endl;
#endif

        using namespace std;
        bool     use_pipe = argc == 2 && argv[1] == "--pipe"s;
        uint32_t selected_button_index =
            use_pipe ? msg_box.show() : msg_box.show_in_child_process();
        std::cout << "user select button: " << selected_button_index
                  << std::endl;
    }
    //catch (std::exception& e)
    {
      //  std::cerr << "error: " << e.what() << std::endl;
      //  return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
namespace om
{
om::gui::msg_box parse_args_or_stdin(int argc, char** argv)
{
    om::gui::msg_box msg_box;

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
        std::exit(EXIT_SUCCESS);
    }

    if (vm.count("button"))
    {
        auto values = vm["button"].as<std::vector<std::string>>();
        for (auto& v : values)
        {
            msg_box.add_button(v);
        }
    }

    if (vm.count("text"))
    {
        auto str = vm["text"].as<std::string>();
        msg_box.text(str);
    }

    if (vm.count("title"))
    {
        auto str = vm["title"].as<std::string>();
        msg_box.title(str);
    }

    if (vm.count("pipe"))
    {
        std::cout << "start pipe" << std::endl;
        std::string command;
        for (std::cin >> command; !command.empty(); std::cin >> command)
        {
            if (command == "button")
            {
                std::string button;
                std::cin >> button;
                std::cout << "add button: " << button << std::endl;
                msg_box.add_button(button);
            }
            else if (command == "text")
            {
                std::string line;
                std::getline(std::cin, line);
                std::cout << "add text: " << line << std::endl;
                msg_box.text(line);
            }
            command.clear();
        }
    }
    return msg_box;
}
} // namespace om
