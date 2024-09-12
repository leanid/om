export module hello;

import std;

namespace ask
{
export void greeter(std::string_view const& name)
{
    std::cout << "Hi " << name << "!\n";
}
} // namespace ask
