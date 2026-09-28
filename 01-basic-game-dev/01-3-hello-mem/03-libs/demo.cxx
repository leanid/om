#include "demo.hxx"

#include "banned.hxx"
#include "maps.hxx"

#include <new>

namespace om
{

void run_allocation_demos(const greedy_api& api) noexcept
{
    // 1. глобальный new внутри библиотеки. Наши определения operator new
    //    сильнее weak-версий из libstdc++/libc++, поэтому линкер и
    //    динамический загрузчик отдают вызовы нам - даже из чужого кода.
    try
    {
        char* p = api.do_new(64);
        print("  new:    ERROR: сработал, ptr=");
        print_ptr(p);
        print("\n");
    }
    catch (const std::bad_alloc&)
    {
        print("  new:    ok: std::bad_alloc - запрет действует "
              "и внутри библиотеки\n");
    }

    // 2. malloc внутри библиотеки. Символ из исполняемого файла
    //    интерпозируется поверх libc для всего процесса.
    if (void* p = api.do_malloc(64); p == nullptr)
    {
        print("  malloc: ok: nullptr - интерпозиция покрывает и её\n");
    }
    else
    {
        print("  malloc: ERROR: сработал, ptr=");
        print_ptr(p);
        print("\n");
    }

    // 3. mmap - запрет работает на уровне C/C++ API, а не системных
    //    вызовов, поэтому прямой mmap из библиотеки успешен.
    if (void* m = api.do_mmap(4096); m != nullptr)
    {
        print("  mmap:   ok: ");
        print_ptr(m);
        print(" - syscall'ы запрет не накрывает\n"
              "          сегмент: ");
        print_map_line_for(m, snapshot_maps(1));
    }
    else
    {
        print("  mmap:   ERROR: не сработал\n");
    }

    // 4. собственный статический пул библиотеки (её .bss): куча не
    //    нужна вообще - так third-party код выживает под запретом.
    if (char* s = api.do_pool(128); s != nullptr)
    {
        print("  pool:   ok: ");
        print_ptr(s);
        print("\n          сегмент-владелец: ");
        print_map_line_for(s, snapshot_maps(1));
    }
    else
    {
        print("  pool:   ERROR: не сработал\n");
    }
}

} // namespace om
