#include <iostream>

int main(int, char**)
{
    using std::cout;
    using std::endl;
    cout << "cout_main.cxx hello world, from c++" << endl;
    return cout.fail();
}
