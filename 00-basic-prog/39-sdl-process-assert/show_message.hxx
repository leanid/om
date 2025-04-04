#pragma once

#include <cstdint>
#include <ostream>
#include <span>
#include <string>
#include <vector>

namespace om::gui
{
/// @brief show gui message with text and caption and text on buttons
/// @return uint32_t - index of button user press
/// @note on error throw std::runtime_error with message from SDL
uint32_t show_message(std::string            title,
                      std::string            text,
                      std::span<std::string> buttons_text);

/// @brief MessageBox class frontend to call from parent process
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
    void title(std::string);
    void text(std::string);
    void add_button(std::string, button_flag flag = button_flag::none);
    void mode(button_mode);

    /// @brief display OS specific dialog with custom icon, title, text and
    /// buttons
    /// @returns index of pressed key
    /// @throw std::runtime_error with error message if can't display dialog
    [[nodiscard]] uint32_t show();

    /// @brief display OS specific dialog as show() but in new process
    /// @thorw std::runtime_error with error message if can't display dialog
    uint32_t show_in_child_process();

private:
    friend std::ostream& operator<<(std::ostream&, const msg_box&);

    struct button
    {
        std::string name;
        button_flag flag;
    };

    std::string         title_;
    std::string         text_;
    std::vector<button> buttons_;
    icon                icon_;
    button_mode         mode_;
};
std::ostream& operator<<(std::ostream&, const msg_box&);

inline void msg_box::type(icon a_icon)
{
    icon_ = a_icon;
}

inline void msg_box::title(std::string title)
{
    title_ = title;
}

inline void msg_box::text(std::string text)
{
    text_ = text;
}

inline void msg_box::add_button(std::string name, button_flag flag)
{
    buttons_.emplace_back(name, flag);
}

inline void msg_box::mode(msg_box::button_mode mode)
{
    mode_ = mode;
}

} // namespace om::gui
