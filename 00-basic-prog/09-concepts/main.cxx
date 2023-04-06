#include <concepts>
#include <cstdlib>
#include <iostream>

// Declaration of the concept "Hashable", which is satisfied by
// any type T such that for values a of type T,
// the expression std::hash<T>{}(a) compiles and its result is convertible to
// std::size_t
template <typename T>
concept Hashable = requires(T a) {
                       {
                           std::hash<T>{}(a)
                           } -> std::convertible_to<std::size_t>;
                   };

void some_func();

int main(int argc, char* argv[])
{
    using namespace std;

    return cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}

void some_func()
{
    using namespace std;
    cout << "just some function call" << endl;
}
