extern "C" long syscall(long call_index, ...);

constexpr long SYS_write = 1;  // from <sys/syscall.h>
constexpr long SYS_exit  = 60; // from <sys/syscall.h>

int print(const char* message, int size)
{
    long exit_code = syscall(SYS_write, 1, message, size);
    return static_cast<int>(exit_code);
}

int main(int /*argc*/, char* /*argv*/[])
{
    return print("hack_asm_main.cxx hello world\n", 12);
}

extern "C"
{
    void _start()
    {
        int result_code = main(0, nullptr);
        syscall(SYS_exit, result_code);
    }
}
