import std;
import glm;
import vulkan_hpp;

int main()
{
    using namespace std::numbers;
    glm::mat4 m{ 1.f };
    glm::vec3 v{ 1.f };

    float     up   = glm::degrees(pi / 2.f);
    float     pi_2 = glm::radians(up);
    glm::mat4 mv   = glm::ext::translate(m, v);

    std::cout << "print from import glm;" << std::endl;
    std::cout << m[0][0] << std::endl;
    std::cout << mv[3][0] << std::endl;
    std::cout << v[0] << std::endl;
    std::cout << up << std::endl;
    std::cout << pi_2 << std::endl;

    try
    {
        vk::detail::DispatchLoaderDynamic dynamic_loader;
        vk::InstanceCreateInfo            create_info{};
        vk::Instance                      instance =
            vk::createInstance(create_info, nullptr, dynamic_loader);
        std::cout << "vulkan instance created\n";
    }
    catch (std::exception& ex)
    {
        std::cerr << "can't create vk::Instance: " << ex.what() << std::endl;
    }

    return std::cout.fail();
}
