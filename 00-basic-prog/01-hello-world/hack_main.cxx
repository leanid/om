#if defined(__linux__)

#include <sys/syscall.h>
#include <unistd.h>

long int print(const char* message, int size)
{
    constexpr int stdout_stream = 1;
    long int      exit_code = syscall(SYS_write, stdout_stream, message, size);
    return exit_code;
}

int main(int /*argc*/, char* /*argv*/[])
{
    return print("hello world\n", 12);
}

extern "C"
{
    void _start()
    {
        int result_code = main(0, nullptr);
        syscall(SYS_exit, result_code);
    }
}

#endif // defined(__linux__)
