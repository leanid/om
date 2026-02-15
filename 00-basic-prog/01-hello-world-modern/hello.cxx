import std;

int main()
{
    try
    {
        std::println(std::cout, "hello world");
        std::cout << std::flush;
        return std::cout.fail();
    }
    catch (...)
    {
        return 1;
    }
}
