#include <cstdint>
#include <iostream>

#include <vulkan/vulkan.hpp>

int main(int argc, char** argv)
{
    uint32_t num_extensions{};
    std::ignore = vk::enumerateInstanceExtensionProperties(
        nullptr, &num_extensions, nullptr);
    std::cout << "num of vulkan extension on my machine: [" << num_extensions
              << "]" << std::endl;
    return std::cout.fail();
}
