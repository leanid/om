#include <csignal>
#include <iostream>

extern "C" void custom_handler(int sig) noexcept(false)
{
    // DO NOT DO IT AT HOME (this is undefined behaviour)
    std::cerr << "we in " << __FUNCTION__ << " with sig: " << sig << std::endl;
    throw std::runtime_error("try overcome signal");
}

int main()
{
    std::cout << "start" << std::endl;
    if (SIG_ERR == std::signal(SIGSEGV, custom_handler))
    {
        std::cout << "can't install custom_handler" << std::endl;
        return -1;
    }

    try
    {
        int* bad_ptr = reinterpret_cast<int*>(0xBAD);
        int  val     = *bad_ptr;
        std::cout << bad_ptr << ": " << val << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << "first time got exception: " << ex.what();
    }

    std::cout << "do you see any exception" << std::endl;

    try
    {
        int* bad_ptr = reinterpret_cast<int*>(0xBAD);
        int  val     = *bad_ptr;
        std::cout << bad_ptr << ": " << val << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << "second time got exception: " << ex.what();
    }

    std::cout << "finish it" << std::endl;
    return 0;
}
