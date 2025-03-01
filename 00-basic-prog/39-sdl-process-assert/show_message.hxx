#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace om::gui
{
/// @brief show gui message with text and caption and text on buttons
/// @return uint32_t - index of button user press
/// @note on error throw std::runtime_error with message from SDL
uint32_t show_message(std::u8string_view            title,
                      std::u8string_view            text,
                      std::span<std::u8string_view> buttons_text);

class msg_box
{
public:
    enum class icon
    {
        error,
        warning,
        info
    };
    enum class button_flag
    {
        none,
        return_key_default_button,
        escape_key_default_button
    };
    enum class button_mode
    {
        left_to_right,
        right_to_left
    };
    void type(icon);
    void title(std::u8string);
    void text(std::u8string);
    void add_button(std::u8string, button_flag flag = button_flag::none);
    void mode(button_mode);

    /// @brief display OS specific dialog with custom icon, title, text and
    /// buttons
    /// @returns index of pressed key
    /// @throw std::runtime_error with error message in can't display dialog
    [[nodiscard]] uint32_t show();

private:
    struct button
    {
        std::u8string name;
        button_flag   flag;
    };

    std::u8string       title_;
    std::u8string       text_;
    std::vector<button> buttons_;
    icon                icon_;
    button_mode         mode_;
};

} // namespace om::gui
