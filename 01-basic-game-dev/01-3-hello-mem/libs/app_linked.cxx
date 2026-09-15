#include "banned.hxx"
#include "demo.hxx"
#include "maps.hxx"

#include "greedy_lib.hxx"

#ifndef VARIANT
#define VARIANT "unknown"
#endif

int main()
{
    om::print("=== 01-3-mem-libs [" VARIANT "] ===\n");

    // Для .so сегменты библиотеки уже в карте с самого старта (их
    // отобразил ld.so ещё до main). Для статической библиотеки
    // отдельного модуля в карте нет вообще: её код и данные влиты в
    // сегменты исполняемого файла.
    om::print("-- строки /proc/self/maps со словом \"greedy\" на старте:\n");
    om::print_maps_containing(om::snapshot_maps(0), "greedy");

    om::print("-- четыре способа выделить память внутри библиотеки:\n");
    om::run_allocation_demos(
        { &greedy_new, &greedy_malloc, &greedy_mmap, &greedy_pool });

    om::print("=== done ===\n");
    return 0;
}
