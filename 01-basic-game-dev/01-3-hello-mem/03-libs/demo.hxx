#pragma once

#include <cstddef>

namespace om
{

using alloc_new_fn    = char* (*)(std::size_t);
using alloc_malloc_fn = void* (*)(std::size_t);
using alloc_mmap_fn   = void* (*)(std::size_t);
using alloc_pool_fn   = char* (*)(std::size_t);

/// Указатели на функции библиотеки. Как они получены - прямой линковкой
/// или dlsym - для демонстрации неважно, поведение одинаковое.
struct greedy_api
{
    alloc_new_fn    do_new;
    alloc_malloc_fn do_malloc;
    alloc_mmap_fn   do_mmap;
    alloc_pool_fn   do_pool;
};

/// Прогоняет четыре способа выделения памяти внутри библиотеки и
/// печатает, что сработало, а что попало под запрет процесса.
void run_allocation_demos(const greedy_api& api) noexcept;

} // namespace om
