#include <cstdint>
#include <iostream>

#include <vulkan/vulkan.hpp>

int main(int argc, char** argv)
{
    uint32_t num_extensions{};
    vk::Result r = vk::enumerateInstanceExtensionProperties(
        nullptr, &num_extensions, nullptr);
    if (vk::Result::eSuccess != r)
    {
        std::cerr << "error: failed to enumerate extensions\n";
    }
    std::cout << "num of vulkan extension on my machine: [" << num_extensions
              << "]" << std::endl;
    return std::cout.fail();
}
