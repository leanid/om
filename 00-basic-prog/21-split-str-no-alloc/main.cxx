#include <iomanip>
#include <iostream>
#include <string_view>
#include <utility>

struct split_by_spaces
{
    std::string_view      text;
    static constexpr char delim = ' ';

    struct iterator
    {
        const std::string_view& text;
        std::size_t             cur_pos;
        std::size_t             end_pos;

        std::string_view operator*() const
        {
            return { &text[cur_pos], end_pos - cur_pos };
        }
        bool operator==(const iterator& other) const
        {
            return cur_pos == other.cur_pos && end_pos == other.end_pos;
        }
        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }
        iterator& operator++()
        {
            cur_pos = text.find_first_not_of(delim, end_pos);

            if (cur_pos == std::string_view::npos)
            {
                cur_pos = text.size();
                end_pos = cur_pos;
                return *this;
            }

            end_pos = text.find(delim, cur_pos);

            if (cur_pos == std::string_view::npos)
            {
                end_pos = text.size();
            }

            return *this;
        }
    };

    [[nodiscard]] iterator begin() const
    {
        auto start = text.find_first_not_of(delim);
        if (start == std::string_view::npos)
        {
            return iterator{ text, text.size(), text.size() };
        }
        auto end_word = text.find(delim, start);
        if (end_word == std::string_view::npos)
        {
            end_word = text.size();
        }
        return iterator{ text, start, end_word };
    }
    [[nodiscard]] iterator end() const
    {
        return iterator{ text, text.size(), text.size() };
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    using namespace std::literals;
    auto str = " there should be no memory allocation during parsing"
               "  into words this line and you   shouldn't create any"
               "  container                  for intermediate words  "sv;

    auto comma = "";
    for (std::string_view word : split_by_spaces{ str })
    {
        std::cout << std::exchange(comma, ",") << std::quoted(word);
    }

    auto only_spaces = "                   "sv;
    for ([[maybe_unused]] std::string_view word : split_by_spaces{ only_spaces })
    {
        std::cout << "you will not see this line in output" << std::endl;
    }
}
