#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace om
{

/// Асинхронно-безопасный вывод в stderr без кучи (только write(2)).
void log_to_stderr(std::string_view text) noexcept;

/// Печать в stdout без кучи: iostream/printf могут внутри себя звать
/// malloc, который в этих программах запрещён.
void print(std::string_view text) noexcept;
void print_u64(std::uint64_t value) noexcept;
void print_ptr(const void* ptr) noexcept;

/// Временно включает/выключает запрет семейства malloc. Нужно для
/// демонстрации dlopen: современный ld.so (glibc >= 2.35) после старта
/// процесса пользуется интерпозируемым malloc, поэтому под тотальным
/// запретом dlopen падает с "out of memory".
void set_malloc_ban(bool enabled) noexcept;

} // namespace om
