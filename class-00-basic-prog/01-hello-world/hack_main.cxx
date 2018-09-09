#include <sys/syscall.h>
#include <unistd.h>

int print(const char* message, int size)
{
    int exit_code = 0;
    syscall(SYS_write, 1, message, size);
    return exit_code;
}

int main(int /*argc*/, char* /*argv*/ [])
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
