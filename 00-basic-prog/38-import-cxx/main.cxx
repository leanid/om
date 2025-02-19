// import std;
#include <iostream>
#include <numbers>
// import glm;
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_hpp_macros.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.hpp>
// import vulkan_hpp;

int main()
{
    using namespace std::numbers;
    glm::mat4 m{ 1.f };
    glm::vec3 v{ 1.f };

    float     up   = glm::degrees(pi / 2.f);
    float     pi_2 = glm::radians(up);
    glm::mat4 mv   = glm::translate(m, v);

    std::cout << "print from import glm;" << std::endl;
    std::cout << m[0][0] << std::endl;
    std::cout << mv[3][0] << std::endl;
    std::cout << v[0] << std::endl;
    std::cout << up << std::endl;
    std::cout << pi_2 << std::endl;

    try
    {
        vk::detail::DynamicLoader dynamic_loader;
        auto                      vkGetInstanceProcAddr =
            dynamic_loader.getProcAddress<PFN_vkGetInstanceProcAddr>(
                "vkGetInstanceProcAddr");

        if (!vkGetInstanceProcAddr)
        {
            throw std::runtime_error("Failed to load vkGetInstanceProcAddr");
        }
        std::cout << "pointer to vkGetInstanceProcAddr resieved" << std::endl;

        vk::detail::DispatchLoaderDynamic dispatch_loader_dynamic(
            vkGetInstanceProcAddr);

        uint32_t instanceVersion =
            vk::enumerateInstanceVersion(dispatch_loader_dynamic);

        uint32_t major = vk::apiVersionMajor(instanceVersion);
        uint32_t minor = vk::apiVersionMinor(instanceVersion);
        uint32_t patch = vk::apiVersionPatch(instanceVersion);
        std::cout << "vulkan version: " << major << '.' << minor << '.' << patch
                  << std::endl;

        vk::ApplicationInfo app_info{
            "My App",                       // Название приложения
            1,                              // Версия приложения
            nullptr,                        // Название движка (опционально)
            vk::makeApiVersion(0, 1, 0, 0), // Версия движка
            vk::makeApiVersion(0, 1, 0, 0)  // Версия Vulkan API
        };
        vk::InstanceCreateInfo create_info{
            {},                 // Флаги
            &app_info,          // Обязательная информация о приложении
            0,         nullptr, // Слои (debug и т.д.)
            0,         nullptr  // Расширения экземпляра
        };
        vk::Instance instance =
            vk::createInstance(create_info, nullptr, dispatch_loader_dynamic);
        std::cout << "vulkan instance created\n";
    }
    catch (std::exception& ex)
    {
        std::cerr << "can't create vk::Instance: " << ex.what() << std::endl;
    }

    return std::cout.fail();
}
