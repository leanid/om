#include <cstdlib>
#include <iostream>
#include <new>

static int global_counter = 0;

void* operator new(std::size_t n) noexcept(
    false) // _THROW_BAD_ALLOC // throw(std::bad_alloc)
{
    void* result = malloc(n);
    ++global_counter;
    return result;
}
void operator delete(void* p) noexcept(true)
{
    free(p);
    --global_counter;
}
void operator delete(void* p, std::size_t) noexcept(true)
{
    free(p);
    --global_counter;
}

struct A
{
    A(int i)
        : value(i)
    {
        std::cout << "constructor A" << std::endl;
    }
    [[noreturn]]~A() noexcept(false) {
        std::cout << "in destructor" << std::endl;
        throw value;
    }

    int value;
};

int main(int, char**)
{
    std::cout << "start global_counter = " << global_counter << std::endl;
    {
        try
        {
            // A a1();
            A* a2{ new A(2) };
            std::cout << "before ex global_counter = " << global_counter
                      << std::endl;
            delete a2;
        }
        catch (int i)
        {
            std::cout << "in catch ex global_counter = " << global_counter
                      << std::endl;
            std::cout << "exception value: " << i << std::endl;
        }
    }
    std::cout << "end global_counter = " << global_counter << std::endl;
    return 0;
}
