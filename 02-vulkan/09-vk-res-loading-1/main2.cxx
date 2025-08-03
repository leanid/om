import std;
import vulkan_hpp;
import glm;

std::string to_version(std::uint32_t v);

int main(int argc, char** argv)
{
    try
    {
        std::println("am I dreaming or it's works!");
        std::cout << "hello from main2.cxx\n";
        std::uint32_t maximum_supported = vk::enumerateInstanceVersion();
        std::cout << "vk api version: " << to_version(maximum_supported)
                  << " maximum supported\n";
        glm::mat4 m{ 1.f };
        glm::vec3 v{ 1.f };

        float     up   = glm::degrees(std::numbers::pi / 2.f);
        float     pi_2 = glm::radians(up);
        glm::mat4 mv   = glm::translate(m, v);

        std::cout << "print from import glm;" << std::endl;
        std::cout << m[0][0] << std::endl;
        std::cout << mv[3][0] << std::endl;
        std::cout << v[0] << std::endl;
        std::cout << up << std::endl;
        std::cout << pi_2 << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}

std::string to_version(std::uint32_t v)
{
    auto major = (((std::uint32_t)(v) >> 22U) & 0x7FU);
    auto minor = (((std::uint32_t)(v) >> 12U) & 0x3FFU);
    auto patch = ((std::uint32_t)(v) & 0xFFFU);
    return std::to_string(major) + '.' + std::to_string(minor) + '.' +
           std::to_string(patch);
}
