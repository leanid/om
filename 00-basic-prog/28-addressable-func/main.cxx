#include <iostream>
#include <ostream>

int main(int argc, char** argv)
{
    std::cout << "hello world\n";
    std::cout << std::flush; // it works but how?
                             // cout has operator << overloading for function
                             // pointers std::flush - and other manipulators -
                             // "addressable functions"

    // OK how to add my custom manipulators?

    auto line_stars = [](std::ostream& os) -> std::ostream&
    {
        os << "***   ***   ***   ***   ***   ***   ***";
        return os;
    };

    // now I use my custom "io manipulator"
    std::cout << line_stars << std::endl;
    return 0;
}
