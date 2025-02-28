#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string_view>
#include <system_error>

namespace om
{
/// @brief show gui message with text and caption and text on buttons
/// @return uint32_t - index of button user press
std::expected<uint32_t, std::error_code> show_message(
    std::u8string_view            caption,
    std::u8string_view            text,
    std::span<std::u8string_view> buttons_text);
} // namespace om
