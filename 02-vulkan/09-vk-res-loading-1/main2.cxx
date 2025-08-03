import std;
import vulkan_hpp;

#define VK_VERSION_MAJOR(version) (((std::uint32_t)(version) >> 22U) & 0x7FU)
#define VK_VERSION_MINOR(version) (((std::uint32_t)(version) >> 12U) & 0x3FFU)
#define VK_VERSION_PATCH(version) ((std::uint32_t)(version) & 0xFFFU)

int main(int argc, char** argv)
{
    try
    {
        std::println("am I dreaming or it's works!");
        std::cout << "hello from main2.cxx\n";
        std::uint32_t maximum_supported = vk::enumerateInstanceVersion();
        std::cout << "vk api version: " << VK_VERSION_MAJOR(maximum_supported)
                  << '.' << VK_VERSION_MINOR(maximum_supported) << '.'
                  << VK_VERSION_PATCH(maximum_supported)
                  << " maximum supported\n";
    }
    catch (std::exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
