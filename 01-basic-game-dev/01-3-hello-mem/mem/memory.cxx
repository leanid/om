#include "memory.hxx"

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <new>

#include <sys/mman.h>
#include <unistd.h>

namespace om
{

namespace detail
{

void log_to_stderr(std::string_view text) noexcept
{
    while (!text.empty())
    {
        const ssize_t written =
            ::write(STDERR_FILENO, text.data(), text.size());
        if (written <= 0)
        {
            return;
        }
        text.remove_prefix(static_cast<std::size_t>(written));
    }
}

void opaque_escape(const void*) noexcept {}

} // namespace detail

namespace
{

/// Заголовок каждой аллокации: лежит прямо перед указателем
/// пользователя, чтобы deallocate знал, что отдавать в munmap.
struct allocation_header
{
    void*       mapping_base;
    std::size_t mapping_size;
};

static_assert(sizeof(allocation_header) == 16,
              "header must keep user pointer 16-byte aligned");

allocation_stats g_stats;

} // namespace

void* mmap_allocator::allocate(std::size_t bytes, std::size_t alignment)
{
    constexpr std::size_t min_alignment = sizeof(allocation_header);

    if (bytes == 0)
    {
        bytes = 1;
    }
    if (alignment < min_alignment)
    {
        alignment = min_alignment;
    }
    if ((alignment & (alignment - 1)) != 0)
    {
        detail::log_to_stderr(
            "mmap_allocator: alignment must be a power of 2\n");
        std::abort();
    }

    const std::size_t total = bytes + alignment + sizeof(allocation_header);

    void* base = ::mmap(nullptr,
                        total,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1,
                        0);
    if (base == MAP_FAILED)
    {
        detail::log_to_stderr("mmap_allocator: mmap failed\n");
        std::abort();
    }

    const auto raw =
        reinterpret_cast<std::uintptr_t>(base) + sizeof(allocation_header);
    const auto user_addr =
        (raw + (alignment - 1)) & ~static_cast<std::uintptr_t>(alignment - 1);

    auto* header         = reinterpret_cast<allocation_header*>(user_addr) - 1;
    header->mapping_base = base;
    header->mapping_size = total;

    using std::memory_order_relaxed;
    g_stats.allocation_count.fetch_add(1, memory_order_relaxed);
    g_stats.bytes_in_use.fetch_add(total, memory_order_relaxed);
    g_stats.bytes_total.fetch_add(total, memory_order_relaxed);

    return reinterpret_cast<void*>(user_addr);
}

void mmap_allocator::deallocate(void* ptr) noexcept
{
    if (ptr == nullptr)
    {
        return;
    }

    const auto* header = reinterpret_cast<const allocation_header*>(ptr) - 1;
    void*       base   = header->mapping_base;
    const std::size_t size = header->mapping_size;

    using std::memory_order_relaxed;
    g_stats.deallocation_count.fetch_add(1, memory_order_relaxed);
    g_stats.bytes_in_use.fetch_sub(size, memory_order_relaxed);

    if (::munmap(base, size) != 0)
    {
        detail::log_to_stderr("mmap_allocator: munmap failed\n");
        std::abort();
    }
}

const allocation_stats& mmap_allocator::stats() noexcept
{
    return g_stats;
}

void* mmap_memory_resource::do_allocate(std::size_t bytes,
                                        std::size_t alignment)
{
    return mmap_allocator::allocate(bytes, alignment);
}

void mmap_memory_resource::do_deallocate(void* ptr,
                                         std::size_t /*bytes*/,
                                         std::size_t /*alignment*/)
{
    mmap_allocator::deallocate(ptr);
}

bool mmap_memory_resource::do_is_equal(
    const std::pmr::memory_resource& other) const noexcept
{
    return this == &other;
}

} // namespace om

//////////////////////////////////////////////////////////////////////////
// Запрет глобальных operator new/delete.
//
// Наши определения сильнее слабых (weak) версий из libstdc++, поэтому
// линкер берёт именно их. Любая попытка выделить память через глобальный
// new (в т.ч. внутри std::string, std::vector без pmr и т.п.) бросает
// std::bad_alloc, а nothrow-варианты возвращают nullptr.
//////////////////////////////////////////////////////////////////////////

namespace
{

[[noreturn]] void banned_operator_new()
{
    om::detail::log_to_stderr(
        "banned: global operator new - use om::mmap_memory_resource\n");
    throw std::bad_alloc();
}

void* banned_operator_new_nothrow() noexcept
{
    om::detail::log_to_stderr("banned: nothrow global operator new\n");
    return nullptr;
}

} // namespace

void* operator new(std::size_t)
{
    banned_operator_new();
}
void* operator new[](std::size_t)
{
    banned_operator_new();
}
void* operator new(std::size_t, std::align_val_t)
{
    banned_operator_new();
}
void* operator new[](std::size_t, std::align_val_t)
{
    banned_operator_new();
}

void* operator new(std::size_t, const std::nothrow_t&) noexcept
{
    return banned_operator_new_nothrow();
}
void* operator new[](std::size_t, const std::nothrow_t&) noexcept
{
    return banned_operator_new_nothrow();
}
void* operator new(std::size_t,
                   std::align_val_t,
                   const std::nothrow_t&) noexcept
{
    return banned_operator_new_nothrow();
}
void* operator new[](std::size_t,
                     std::align_val_t,
                     const std::nothrow_t&) noexcept
{
    return banned_operator_new_nothrow();
}

// delete-варианты обязаны существовать (линковка), но до них никогда не
// доходит живой указатель: new всегда бросает. delete(nullptr) - нооп.
void operator delete(void*) noexcept {}
void operator delete[](void*) noexcept {}
void operator delete(void*, std::size_t) noexcept {}
void operator delete[](void*, std::size_t) noexcept {}
void operator delete(void*, std::align_val_t) noexcept {}
void operator delete[](void*, std::align_val_t) noexcept {}
void operator delete(void*, std::size_t, std::align_val_t) noexcept {}
void operator delete[](void*, std::size_t, std::align_val_t) noexcept {}
void operator delete(void*, const std::nothrow_t&) noexcept {}
void operator delete[](void*, const std::nothrow_t&) noexcept {}
void operator delete(void*, std::align_val_t, const std::nothrow_t&) noexcept {}
void operator delete[](void*, std::align_val_t, const std::nothrow_t&) noexcept
{
}

//////////////////////////////////////////////////////////////////////////
// Запрет C-кучи: malloc и компания.
//
// Символы из исполняемого файла интерпозируются поверх libc: любой вызов
// malloc (даже из чужих .so) попадает сюда. Возвращаем nullptr, а не
// abort(), потому что __cxa_allocate_exception в libstdc++ сначала зовёт
// malloc, а при неудаче откатывается на свой аварийный статический буфер -
// так исключения (в т.ч. наш std::bad_alloc) продолжают работать.
//////////////////////////////////////////////////////////////////////////

extern "C"
{

    void* malloc(std::size_t) noexcept
    {
        om::detail::log_to_stderr("banned: malloc\n");
        return nullptr;
    }

    void free(void*) noexcept {}

    void* calloc(std::size_t, std::size_t) noexcept
    {
        om::detail::log_to_stderr("banned: calloc\n");
        return nullptr;
    }

    void* realloc(void*, std::size_t) noexcept
    {
        om::detail::log_to_stderr("banned: realloc\n");
        return nullptr;
    }

    void* aligned_alloc(std::size_t, std::size_t) noexcept
    {
        om::detail::log_to_stderr("banned: aligned_alloc\n");
        return nullptr;
    }

    int posix_memalign(void**, std::size_t, std::size_t) noexcept
    {
        om::detail::log_to_stderr("banned: posix_memalign\n");
        return ENOMEM;
    }

} // extern "C"
