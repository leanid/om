import std;
import vulkan_hpp;

int main(int argc, char** argv)
{
    try
    {
        std::println("am I dreaming or it's works!");
    }
    catch (std::exception& e)
    {
        std::cerr << e.what();
    }
    std::cout << "hello from main2.cxx\n";
    std::cout << "sizeof(vk::PhysicalDevice): " << sizeof(vk::PhysicalDevice)
              << std::endl;
}
