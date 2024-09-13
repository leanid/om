#include <cstdlib>
#include <iostream>
#include <memory>
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

struct B
{
    B() { std::cout << "constructor B" << std::endl; }
    ~B() { std::cout << "destructor B" << std::endl; }

    void some_method(int arg)
    {
        std::cout << "B is alive and it's this == 0x" << this
                  << " and arg == " << arg << std::endl;
    }
};

struct A
{
    explicit A(int i)
        : value(i)
    {
        std::cout << "constructor A start" << std::endl;
        ptrB = std::make_unique<B>();
        std::cout << "constructor A finish" << std::endl;
    }
    ~A() noexcept(false)
    {
        std::cout << "destructor A start" << std::endl;
        ptrB->some_method(1);
        if (rand() > 0) // always works without seed initialization
        {
            throw value; // NOLINT(hicpp-exception-baseclass)
        }
        ptrB->some_method(2);
        std::cout << "destructor A finish normally" << std::endl;
    }

    std::unique_ptr<B> ptrB;
    int                value;
};

int main(int, char**)
{
    using namespace std;
    cout << "start program [global_counter = " << global_counter << "]" << endl;
    {
        try
        {
            // A a1();
            A* a2{ new A(2) };
            cout << "after constructor a2 [global_counter = " << global_counter
                 << "]" << endl;
            delete a2;
            cout << "after destructor a2" << endl;
        }
        catch (int i)
        {
            cout << "inside catch i [global_counter = " << global_counter << "]"
                 << endl;
            cout << "inside catch exception value: " << i << endl;
        }
    }
    cout << "finish program [global_counter = " << global_counter << "]"
         << endl;
    return 0;
}
