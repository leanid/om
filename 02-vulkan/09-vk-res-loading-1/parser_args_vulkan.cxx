#include "parser_args_vulkan.hxx"

#include <boost/program_options.hpp>

namespace om::tools
{
parser_args_vulkan::parser_args_vulkan(int argc, char** argv)
{
    using namespace boost::program_options;
    std::string         vulkan_version;
    options_description options("options");
    options.add_options()("help,h", "print this help");
    options.add_options()("verbose,v", "enable verbose mode");
    options.add_options()(
        "vk_ver",
        value<std::string>(&vulkan_version)->default_value("1.3"),
        "desired vulkan version in format 1.0 or 1.1 or 1.2 or 1.3 or 1.4");
    options.add_options()("vk_validation_layer,l",
                          "enable VK_LAYER_KHRONOS_validation");
    options.add_options()("vk_debug_callback,d", "enable VK_EXT_debug_utils");

    variables_map vm;
    store(parse_command_line(argc, argv, options), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::stringstream ss;
        ss << options;
        help = ss.str();
        return;
    }

    verbose          = vm.count("verbose");
    validation_layer = vm.count("vk_validation_layer");
    debug_callback   = vm.count("vk_debug_callback");

    // expected format is "1.0" or "1.1" or "1.2" or "1.3" or "1.4"
    auto index_of_point = vulkan_version.find('.');
    if (index_of_point == std::string::npos)
    {
        std::stringstream ss;
        ss << options;
        help = ss.str();
        return;
    }

    vulkan_version_major = std::stoi(vulkan_version.substr(0, index_of_point));
    vulkan_version_minor = std::stoi(vulkan_version.substr(index_of_point + 1));
}
} // namespace om::tools
