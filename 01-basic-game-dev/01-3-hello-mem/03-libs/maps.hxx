#pragma once

#include <cstddef>
#include <string_view>

namespace om
{

/// Читает /proc/self/maps в статический буфер (куча в этих программах
/// запрещена!). slot: 0 или 1 - два независимых снимка для "до/после".
std::string_view snapshot_maps(int slot) noexcept;

/// Печатает строки maps, содержащие needle (например имя .so).
/// Возвращает число найденных строк.
std::size_t print_maps_containing(std::string_view maps,
                                  std::string_view needle) noexcept;

/// Печатает строки newer, которых не было в older (новые сегменты).
/// Чтобы увидеть ИСЧЕЗНУВШИЕ сегменты - поменяй аргументы местами.
void print_maps_added(std::string_view older, std::string_view newer) noexcept;

/// Печатает строку maps - владельца адреса (какой сегмент его содержит).
void print_map_line_for(const void* ptr, std::string_view maps) noexcept;

} // namespace om
