#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace om
{
/// @brief show gui message with text and caption and text on buttons
/// @return uint32_t - index of button user press
/// @note on error throw std::runtime_error with message from SDL
uint32_t show_message(std::u8string_view            title,
                      std::u8string_view            text,
                      std::span<std::u8string_view> buttons_text);
} // namespace om
