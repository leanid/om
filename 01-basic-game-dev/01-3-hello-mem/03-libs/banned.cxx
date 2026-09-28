#include "banned.hxx"

#include <cerrno>
#include <charconv>
#include <cstdlib>
#include <new>

#include <unistd.h>

namespace om
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

} // namespace om

//////////////////////////////////////////////////////////////////////////
// Запрет глобальных operator new/delete.
//
// Наши определения сильнее слабых (weak) версий из libstdc++/libc++,
// поэтому линкер берёт именно их - для всего процесса, включая
// статически и динамически слинкованные библиотеки. Любая попытка
// выделить память через глобальный new бросает std::bad_alloc, а
// nothrow-варианты возвращают nullptr.
//////////////////////////////////////////////////////////////////////////

namespace
{

[[noreturn]] void banned_operator_new()
{
    om::log_to_stderr(
        "banned: global operator new - библиотеке нужен свой пул или mmap\n");
    throw std::bad_alloc();
}

void* banned_operator_new_nothrow() noexcept
{
    om::log_to_stderr("banned: nothrow global operator new\n");
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
// malloc - даже из подгруженной .so - попадает сюда. Возвращаем nullptr,
// а не abort(), потому что __cxa_allocate_exception сначала зовёт
// malloc/aligned_alloc, а при неудаче откатывается на свой аварийный
// статический буфер - так исключения (в т.ч. наш std::bad_alloc)
// продолжают работать.
//
// Запрет можно временно выключить (om::set_malloc_ban): тогда вызовы
// уходят в настоящую кучу glibc через __libc_* - это те же функции,
// минуя нашу интерпозицию. glibc-специфично (на musl их нет), но пример
// и так Linux-only.
//////////////////////////////////////////////////////////////////////////

#include <atomic>

extern "C"
{
    // Внутренние (но экспортируемые) точки входа glibc.
    void* __libc_malloc(std::size_t);
    void  __libc_free(void*);
    void* __libc_calloc(std::size_t, std::size_t);
    void* __libc_realloc(void*, std::size_t);
    void* __libc_memalign(std::size_t, std::size_t);
}

namespace
{

std::atomic<bool> g_malloc_ban_enabled{ true };

} // namespace

namespace om
{

void set_malloc_ban(bool enabled) noexcept
{
    g_malloc_ban_enabled.store(enabled, std::memory_order_relaxed);
}

} // namespace om

extern "C"
{

    void* malloc(std::size_t size) noexcept
    {
        if (g_malloc_ban_enabled.load(std::memory_order_relaxed))
        {
            om::log_to_stderr("banned: malloc\n");
            return nullptr;
        }
        return __libc_malloc(size);
    }

    // free всегда отдаёт в настоящую кучу: в окне ослабленного запрета
    // могли выделиться настоящие блоки, а free(nullptr) - нооп.
    void free(void* p) noexcept
    {
        __libc_free(p);
    }

    void* calloc(std::size_t nmemb, std::size_t size) noexcept
    {
        if (g_malloc_ban_enabled.load(std::memory_order_relaxed))
        {
            om::log_to_stderr("banned: calloc\n");
            return nullptr;
        }
        return __libc_calloc(nmemb, size);
    }

    void* realloc(void* p, std::size_t size) noexcept
    {
        if (g_malloc_ban_enabled.load(std::memory_order_relaxed))
        {
            om::log_to_stderr("banned: realloc\n");
            return nullptr;
        }
        return __libc_realloc(p, size);
    }

    void* aligned_alloc(std::size_t alignment, std::size_t size) noexcept
    {
        if (g_malloc_ban_enabled.load(std::memory_order_relaxed))
        {
            om::log_to_stderr("banned: aligned_alloc\n");
            return nullptr;
        }
        return __libc_memalign(alignment, size);
    }

    int posix_memalign(void**      out,
                       std::size_t alignment,
                       std::size_t size) noexcept
    {
        if (g_malloc_ban_enabled.load(std::memory_order_relaxed))
        {
            om::log_to_stderr("banned: posix_memalign\n");
            return ENOMEM;
        }
        void* p = __libc_memalign(alignment, size);
        if (p == nullptr)
        {
            return ENOMEM;
        }
        *out = p;
        return 0;
    }

} // extern "C"
