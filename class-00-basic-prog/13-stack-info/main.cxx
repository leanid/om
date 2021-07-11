#include <iostream>
#include <memory>

#include "stack_info.hxx"

int main(int, char**)
{
    using namespace std;

    om::stack_info stack_info;

    cout << "full stack size: " << stack_info.get_stack_size() / 1024 << "kB"
         << '\n'
         << "current stack position: "
         << stack_info.get_current_stack_position() << '\n'
         << "free stack size: "
         << stack_info.get_free_stack_memory_size() / 1024 << "kB" << endl;

    return cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
