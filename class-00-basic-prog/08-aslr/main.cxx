#include <cstdlib>
#include <iostream>

void some_func();

int main(int argc, char* argv[])
{
    using namespace std;
    const int i{};
    cout << "&argc = 0x" << hex << &argc << endl;
    cout << "&argv = 0x" << hex << &argv << endl;
    cout << "&i = 0x" << hex << &i << endl;
    cout << "&some_func = " << hex << reinterpret_cast<void*>(&some_func)
         << endl;

    long your_gues = 0x401350;
    if (reinterpret_cast<void*>(&some_func) ==
        reinterpret_cast<void*>(your_gues))
    {
        // you can try to call function if you know it's address
        // Linux x86_64 g++9.2 release build
        void (*func_ptr)(void);
        func_ptr = reinterpret_cast<typeof(func_ptr)>(0x401350);
        func_ptr();
        // as you can see, stack address every time different, but
        // function address every time the same

        // without ASLR (address space layout randomization)
        // you can find out what variable and where in stack it exists
        // so you can try to HACK program by changing return address or
        // value of some stack variables
    }

    return cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}

void some_func()
{
    using namespace std;
    cout << "just some function call" << endl;
}
