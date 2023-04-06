module;
// global module for old school include files
// later maybe gnu c++std lib will support new import keyword for
// standard include files
#include <iostream>
#include <string_view>

export module hello;

namespace ask
{
export void greeter(std::string_view const& name)
{
    std::cout << "Hi " << name << "!\n";
}
} // namespace ask
