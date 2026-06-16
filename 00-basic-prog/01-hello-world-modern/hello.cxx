import std;

int main()
{
    std::println(std::cout, "hello world");
    std::cout.flush();
    return std::cout.fail();
}
