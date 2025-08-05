module;
#include <boost/program_options.hpp>
#include <exception>

export module vulkan_args_parser;
import std;

namespace om::vulkan
{
export struct args_parser
{
    args_parser(int argc, char** argv);

    std::string   help                 = "";
    std::uint32_t vulkan_version_major = 0;
    std::uint32_t vulkan_version_minor = 0;
    bool          verbose              = false;
    bool          validation_layer     = false;
    bool          debug_callback       = false;
};

args_parser::args_parser(int argc, char** argv)
{
    try
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
        options.add_options()("vk_debug_callback,d",
                              "enable VK_EXT_debug_utils");

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

        auto major  = vulkan_version.substr(0, index_of_point);
        auto minor  = vulkan_version.substr(index_of_point + 1);
        auto to_u32 = [&](std::string s) -> std::uint32_t
        {
            try
            {

                int v = std::stoi(s);
                if (v < 0)
                {
                    throw std::invalid_argument(s);
                }
                return static_cast<uint32_t>(v);
            }
            catch (const std::exception& ex)
            {
                std::stringstream ss;
                ss << "error: bad version number: [" << s << "]\n";
                ss << options;
                help = ss.str();
            }
            return std::numeric_limits<uint32_t>::max();
        };
        vulkan_version_major = to_u32(major);
        vulkan_version_minor = to_u32(minor);
    }
    catch (std::exception& e)
    {
        help = e.what();
    }
    catch (...)
    {
        help = "unknown exception during args parsing";
    }
}
} // namespace om::vulkan
