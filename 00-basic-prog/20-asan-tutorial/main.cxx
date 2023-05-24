#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "hello world" << std::endl;
    int* i = new int(42); // never free - memory leak
    std::cout << *i << std::endl;

    char* str = new char[256];
    str[255]  = '\0';
    std::cout << str;
    delete[] str;
    str[255] = 'A'; // use after free

    return std::cout.fail();
}
