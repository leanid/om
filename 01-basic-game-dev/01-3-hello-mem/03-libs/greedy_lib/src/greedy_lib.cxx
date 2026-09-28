#include "greedy_lib.hxx"

#include <cstdlib>

#include <sys/mman.h>

namespace
{

// Статический пул: живёт в .bss библиотеки, куча не нужна.
char        g_pool[1024 * 1024];
std::size_t g_pool_used = 0;

} // namespace

char* greedy_new(std::size_t bytes)
{
    return new char[bytes]; // глобальный operator new - какой есть в процессе
}

void* greedy_malloc(std::size_t bytes)
{
    return std::malloc(bytes); // какой malloc подхватит линкер - тот и будет
}

void* greedy_mmap(std::size_t bytes)
{
    void* p = ::mmap(nullptr,
                     bytes,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1,
                     0);
    return (p == MAP_FAILED) ? nullptr : p;
}

char* greedy_pool(std::size_t bytes)
{
    if (bytes > sizeof(g_pool) - g_pool_used)
    {
        return nullptr;
    }
    char* p = g_pool + g_pool_used;
    g_pool_used += bytes;
    return p;
}
