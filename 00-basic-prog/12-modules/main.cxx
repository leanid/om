// this works on MSVC2022
import std;
import hello;

int main(void)
{
    std::cout << "printing some text\n";
    ask::greeter("world");
    std::println("print with std::println");
    return std::cout.fail();
}
