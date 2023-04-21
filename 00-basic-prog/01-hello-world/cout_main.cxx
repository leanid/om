#include <iostream>

int main(int, char**)
{
    using std::cout;
    using std::endl;
    cout << "hello world, from c++" << endl;
    return cout.fail();
}
