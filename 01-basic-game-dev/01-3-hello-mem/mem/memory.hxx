#pragma once

#include <atomic>
#include <cstddef>
#include <memory_resource>
#include <string_view>

namespace om
{

/// Статистика аллокаций - упрощённый аналог AllocationRecords из O3DE.
/// Атомики, чтобы аллокатор был пригоден из любого потока.
struct allocation_stats
{
    std::atomic<std::size_t> allocation_count{ 0 };
    std::atomic<std::size_t> deallocation_count{ 0 };
    std::atomic<std::size_t> bytes_in_use{ 0 };
    std::atomic<std::size_t> bytes_total{ 0 };
};

/// Аллокатор-обёртка над mmap/munmap (Linux-only).
/// Перед указателем пользователя хранит заголовок с параметрами
/// отображения, чтобы deallocate мог корректно вызвать munmap.
/// Не возвращает nullptr: при ошибке пишет в stderr и завершает
/// программу через std::abort().
class mmap_allocator final
{
public:
    /// Выделяет bytes байт с выравниванием alignment (степень двойки).
    static void* allocate(std::size_t bytes, std::size_t alignment);

    static void deallocate(void* ptr) noexcept;

    static const allocation_stats& stats() noexcept;
};

/// std::pmr::memory_resource поверх mmap_allocator - легальный способ
/// пользоваться стандартными контейнерами после того, как глобальные
/// operator new/delete в этой программе запрещены (см. memory.cxx).
class mmap_memory_resource final : public std::pmr::memory_resource
{
private:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    void do_deallocate(void*       ptr,
                       std::size_t bytes,
                       std::size_t alignment) override;

    [[nodiscard]] bool do_is_equal(
        const std::pmr::memory_resource& other) const noexcept override;
};

namespace detail
{
/// Асинхронно-безопасный вывод в stderr без кучи (только write(2)).
void log_to_stderr(std::string_view text) noexcept;

/// "Материализует" указатель для оптимизатора: функция определена в
/// другой единице трансляции, поэтому без LTO компилятор обязан считать,
/// что указатель убегает наружу, и не может удалить/заменить аллокацию
/// (elision new-expression или heap-to-stack для malloc).
void opaque_escape(const void* ptr) noexcept;
} // namespace detail

} // namespace om
