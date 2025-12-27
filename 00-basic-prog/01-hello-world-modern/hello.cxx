import std;

int main()
{
    std::println(std::cout, "hello world");
    std::cout << std::flush;
    return std::cout.fail();
}
