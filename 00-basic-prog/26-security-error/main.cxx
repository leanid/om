#include <cstdio>
#include <cstdlib>
#include <string>

void help_how_to_hack_this_program(int argc, char** argv, char buf[]);

int main(int argc, char** argv)
{
    using namespace std;

    constexpr size_t size              = 1024;
    char             buf[size]         = { 0 };
    char             sub_buf[size / 2] = { 0 };

    std::printf("what is your name: ");

    std::scanf("%511s", sub_buf);

    std::snprintf(buf, 1023, "echo \"your name is %s\"", sub_buf);

    help_how_to_hack_this_program(argc, argv, buf);

    std::system(buf);

    return 0;
}
// just not see below, try first to think yourself
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
void help_how_to_hack_this_program(int argc, char** argv, char buf[])
{
    using namespace std;
    if (argc > 1 && (argv[1] == "--verbose"s || argv[1] == "-v"s))
    {
        // to "hack" this program one may input: "&&uname&&echo"
        std::printf("%s\n", buf);
        std::fflush(stdout);
    }
}
