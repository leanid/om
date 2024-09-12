// this works on MSVC2022 with Ninja generator
import std;
import hello;

int main(void)
{
    std::cout << "printing some text\n";
    ask::greeter("world");
    std::println("print with std::println");
    auto user = std::getenv("USER");
    if (!user)
    {
        user = std::getenv("USERNAME");
    }
    std::println("your name is {}", user);
    return std::cout.fail();
}
