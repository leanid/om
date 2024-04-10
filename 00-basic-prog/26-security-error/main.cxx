#include <cstdio>
#include <cstdlib>
#include <string>

int main(int argc, char** argv)
{
    using namespace std;

    constexpr size_t size          = 1024;
    char             buf[size]     = { 0 };
    char             sub_buf[size] = { 0 };

    std::printf("what is your name: ");

    std::scanf("%1024s", sub_buf);

    std::snprintf(buf, size, "echo \"your name is %s\"", sub_buf);

    if (argc > 1 && (argv[1] == "--verbose"s || argv[1] == "-v"s))
    {
        // to "hack" this program one may input: "&&uname&&echo"
        std::printf("%s\n", buf);
        std::fflush(stdout);
    }

    std::system(buf);

    return 0;
}
