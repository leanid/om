#include <iostream>
#include <memory>

struct type_one
{
    type_one()
        : ptr_i{ new int{} }
    {
        std::cout << "create type_one\n";
        std::cout << "ptr_i == " << ptr_i << std::endl;
    }
    ~type_one()
    {
        std::cout << "destroy type_one\n";
        delete ptr_i;
    }
    std::shared_ptr<type_one> ptr0;
    int*                      ptr_i;
};

int main(int, char**)
{
    using namespace std;

    auto ptr1 = make_shared<type_one>();
    /// circle ref will introduce memory leak
    ptr1->ptr0 = ptr1;
    /// we create tow shared_ptr objects
    /// 1. ptr1 - it is std::shared_ptr<type_one>
    /// 2. inside ptr1 -> type_one
    /// 3. type_one has field std::shared_ptr<type_one> ptr0 - empty after
    /// constructor
    /// 4. now we link inside filed with outside variable

    return cout.fail() ? EXIT_FAILURE : EXIT_SUCCESS;
}
