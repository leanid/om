#include "memory.hxx"

#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h>

namespace
{

/// Вывод без кучи: iostream/printf могут внутри себя звать malloc,
/// который в этой программе запрещён, поэтому пишем напрямую через
/// write(2).
void print(std::string_view text) noexcept
{
    while (!text.empty())
    {
        const ssize_t written =
            ::write(STDOUT_FILENO, text.data(), text.size());
        if (written <= 0)
        {
            return;
        }
        text.remove_prefix(static_cast<std::size_t>(written));
    }
}

void print_u64(std::uint64_t value) noexcept
{
    char       buffer[32];
    const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
    print(std::string_view(buffer,
                           static_cast<std::size_t>(result.ptr - buffer)));
}

void print_ptr(const void* ptr) noexcept
{
    print("0x");
    char       buffer[32];
    const auto result = std::to_chars(buffer,
                                      buffer + sizeof(buffer),
                                      reinterpret_cast<std::uintptr_t>(ptr),
                                      16);
    print(std::string_view(buffer,
                           static_cast<std::size_t>(result.ptr - buffer)));
}

void print_stats(const char* title) noexcept
{
    const om::allocation_stats& stats = om::mmap_allocator::stats();

    print(title);
    print(": allocations=");
    print_u64(stats.allocation_count.load());
    print(" deallocations=");
    print_u64(stats.deallocation_count.load());
    print(" bytes_in_use=");
    print_u64(stats.bytes_in_use.load());
    print(" bytes_total=");
    print_u64(stats.bytes_total.load());
    print("\n");
}

} // namespace

int main()
{
    print("=== 01-3-hello-mem ===\n");

    // 1. hello world вообще без кучи - только syscall write.
    print("hello, memory!\n");

    // 2. std::pmr поверх нашего mmap-аллокатора: стандартные
    //    контейнеры снова работают, хотя глобальный new запрещён.
    om::mmap_memory_resource resource;

    std::pmr::string greeting(&resource);
    greeting = "std::pmr::string живёт в mmap-памяти";
    print(greeting);
    print("\n");

    std::pmr::vector<int> squares(&resource);
    for (int i = 1; i <= 8; ++i)
    {
        squares.push_back(i * i);
    }
    print("squares:");
    for (const int value : squares)
    {
        print(" ");
        print_u64(static_cast<std::uint64_t>(value));
    }
    print("\n");

    // 3. выравнивание больше 16 байт тоже работает.
    struct alignas(64) cache_line
    {
        std::uint64_t data[8];
    };
    std::pmr::vector<cache_line> lines(&resource);
    lines.emplace_back();
    const auto addr = reinterpret_cast<std::uintptr_t>(lines.data());
    print("alignas(64) address is 64-byte aligned: ");
    print((addr % 64 == 0) ? "yes\n" : "NO!\n");

    print_stats("stats after pmr usage");

    // 4. глобальный operator new запрещён - бросает std::bad_alloc.
    //    Оператор вызывается напрямую, как функция, а не через
    //    new-expression: компилятор вправе целиком выкинуть
    //    new-expression, чей результат не "убегает" наружу
    //    ([expr.new]/13, elision) - clang на -O3 так и делает,
    //    а прямой вызов функции выкинуть не может.
    try
    {
        void* banned = ::operator new(64);
        print("ERROR: global new worked, ptr=");
        print_ptr(banned);
        print("\n");
    }
    catch (const std::bad_alloc&)
    {
        print("ok: global operator new threw std::bad_alloc\n");
    }

    // 5. nothrow-вариант тоже запрещён - возвращает nullptr.
    if (void* banned = ::operator new(64, std::nothrow))
    {
        print("ERROR: nothrow new worked, ptr=");
        print_ptr(banned);
        print("\n");
    }
    else
    {
        print("ok: nothrow operator new returned nullptr\n");
    }

    // 6. обычный std::string умирает вместе с new (осторожно: у
    //    коротких строк сработает SSO и аллокации не будет!). Указатель
    //    прогоняется через opaque_escape, иначе на -O3 компилятор
    //    удалит всю конструкцию как мёртвый код.
    try
    {
        const std::string banned(1024, 'x');
        om::detail::opaque_escape(banned.data());
        print("ERROR: std::string allocated!\n");
    }
    catch (const std::bad_alloc&)
    {
        print("ok: long std::string could not allocate\n");
    }

    // 7. malloc и компания запрещены - возвращают nullptr. Без
    //    opaque_escape clang на -O3 подменяет malloc на alloca
    //    (heap-to-stack) и проверка на nullptr вырождается в false.
    void* p = std::malloc(128);
    om::detail::opaque_escape(p);
    print(p == nullptr ? "ok: malloc returned nullptr\n"
                       : "ERROR: malloc worked!\n");

    print_stats("final stats");
    print("=== done ===\n");
    return 0;
}
