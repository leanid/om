#include <sys/syscall.h>
#include <unistd.h>

int main(int /*argc*/, char* /*argv*/ [])
{
    int exit_code = 0;
    syscall(SYS_write, 1, "hello world\n", 12);
    syscall(SYS_exit, exit_code);
}

extern "C"
{
    void _start() { main(0, nullptr); }
}
