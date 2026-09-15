#include "banned.hxx"
#include "demo.hxx"
#include "maps.hxx"

#include <cstddef>

#include <dlfcn.h>

#ifndef GREEDY_SO_PATH
#define GREEDY_SO_PATH "./libgreedy-lib-shared.so"
#endif

namespace
{

template <typename Fn> Fn load(void* handle, const char* name)
{
    // reinterpret_cast между void* и указателем на функцию - POSIX-идиома
    // dlsym (ISO C++ такого каста не знает, но POSIX его гарантирует).
    return reinterpret_cast<Fn>(::dlsym(handle, name));
}

} // namespace

int main()
{
    om::print("=== 01-3-mem-libs [dlopen] ===\n");

    const std::string_view before = om::snapshot_maps(0);

    // ld.so пользуется своим внутренним аллокатором, поэтому dlopen
    // работает, даже когда malloc в процессе запрещён.
    void* handle = ::dlopen(GREEDY_SO_PATH, RTLD_NOW);
    if (handle == nullptr)
    {
        om::print("dlopen failed: ");
        om::print(::dlerror());
        om::print("\n");
        return 1;
    }

    const std::string_view after = om::snapshot_maps(1);
    om::print("-- новые сегменты процесса после dlopen:\n");
    om::print_maps_added(before, after);

    const om::greedy_api api{ load<om::alloc_new_fn>(handle, "greedy_new"),
                              load<om::alloc_malloc_fn>(handle,
                                                        "greedy_malloc"),
                              load<om::alloc_mmap_fn>(handle, "greedy_mmap"),
                              load<om::alloc_pool_fn>(handle, "greedy_pool") };
    if (api.do_new == nullptr || api.do_malloc == nullptr ||
        api.do_mmap == nullptr || api.do_pool == nullptr)
    {
        om::print("dlsym failed\n");
        return 1;
    }

    // Дальше - ровно та же демонстрация, что и у статической и load-time
    // линковки: интерпозиция глобальна для процесса, dlopen её не обходит.
    om::print("-- четыре способа выделить память внутри библиотеки:\n");
    om::run_allocation_demos(api);

    ::dlclose(handle);
    const std::string_view after_close = om::snapshot_maps(0);
    om::print("-- сегменты, исчезнувшие после dlclose:\n");
    om::print_maps_added(after_close, after);

    om::print("=== done ===\n");
    return 0;
}
