#include <climits>
#include <cstddef>
#include <iostream>

namespace om
{
class u8
{
public:
    u8() = default;
    u8(std::byte v)
        : value{ v } {};

private:
    std::byte value = 0;
    friend u8 operator+(const u8& l, const u8& r);
};
u8 operator+(const u8& l, const u8& r)
{
    u8               result;
    constexpr size_t bits_in_byte = CHAR_BIT;
    bool             carry_bit    = 0;
    for (size_t i = 0; i < bits_in_byte; ++i)
    {
        bool left_bit  = (l >> i) & 1;
        bool right_bit = (r >> i) & 1;

        if (left_bit == 1 && right_bit == 1 && carry_bit == 1) // 3
        {
            goto total_bits_3;
        }
        else if (left_bit == 1 && right_bit == 1 && carry_bit == 0) // 2
        {
            goto total_bits_2;
        }
        else if (left_bit == 1 && right_bit == 0 && carry_bit == 1) // 2
        {
            goto total_bits_2;
        }
        else if (left_bit == 0 && right_bit == 1 && carry_bit == 1) // 2
        {
            goto total_bits_2;
        }
        else if (left_bit == 0 && right_bit == 1 && carry_bit == 0) // 1
        {
            goto total_bits_1;
        }
        else if (left_bit == 0 && right_bit == 0 && carry_bit == 1) // 1
        {
            goto total_bits_1;
        }
        else if (left_bit == 1 && right_bit == 0 && carry_bit == 0) // 1
        {
            goto total_bits_1;
        }
        else if (left_bit == 0 && right_bit == 0 && carry_bit == 0) // 0
        {
            goto total_bits_0;
        }

    total_bits_3:
        // set current bit to 1 and carry_bit to 1
    total_bits_2:
        // set current bit to 0 and carry_bit to 1
    total_bits_1:
        // set current bit to 1 and carry_bit to 0
    total_bits_0:
        // set current bit to 0 and carry_bit to 0
    }

    return result;
}
} // end namespace om
