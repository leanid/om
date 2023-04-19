#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

#include "bool.hxx"

namespace om
{

class u8_t;
std::ostream& operator<<(std::ostream& stream, const u8_t& v);

class u8_t
{
public:
    u8_t()            = default;
    u8_t(const u8_t&) = default;
    u8_t(u8_t&&)      = default;
    explicit u8_t(std::byte v)
        : value{ v }
    {
    }
    u8_t& operator=(const u8_t&) = default;

    [[nodiscard]] std::string  to_str() const;
    [[nodiscard]] std::int32_t to_int() const;

    friend u8_t operator+(const u8_t& l, const u8_t& r);
    friend u8_t operator-(const u8_t& l, const u8_t& r);
    // friend u8_t operator<(const u8_t& l, const u8_t& r);

private:
    friend std::ostream& operator<<(std::ostream& stream, const u8_t&);
    // friend std::istream& operator>>(std::istream& stream, u8_t&);

    std::byte value{ 0 };
};
u8_t operator+(const u8_t& l, const u8_t& r)
{
    std::byte result{ 0b00000000 };
    bool_t    carry_bit = false_;

    for (std::byte bit_index_mask{ 1 }; bit_index_mask != std::byte{ 0 };
         bit_index_mask <<= 1)
    {
        bool_t one_bit_l =
            (l.value & bit_index_mask) == std::byte{ 0 } ? false_ : true_;
        bool_t one_bit_r =
            (r.value & bit_index_mask) == std::byte{ 0 } ? false_ : true_;

        bool_t result_bit = carry_bit ^ (one_bit_l ^ one_bit_r);
        carry_bit = (one_bit_l && one_bit_r) || (carry_bit && one_bit_l) ||
                    (carry_bit && one_bit_r);

        if (result_bit)
        {
            result |= bit_index_mask;
        }
    }

    if (carry_bit)
    {
        std::cerr << "overflow result not fit into u8_t" << std::endl;
    }

    return u8_t{ result };
}

u8_t operator-(const u8_t& l, const u8_t& r)
{
    // invert r
    u8_t minus_r{ static_cast<std::byte>(~r.value) };
    minus_r = minus_r + u8_t{ std::byte{ 1 } };

    return l + minus_r;
}

std::ostream& operator<<(std::ostream& stream, const u8_t& v)
{
    stream << "0b";
    for (std::byte i{ 0b10000000 }; i != std::byte{ 0 }; i >>= 1)
    {
        char print_char = (i & v.value) == std::byte{ 0 } ? '0' : '1';
        stream << print_char;
    }
    return stream;
}

std::string u8_t::to_str() const
{
    return std::to_string(static_cast<uint32_t>(value));
}

std::int32_t u8_t::to_int() const
{
    std::int32_t result{ static_cast<int32_t>(value) };
    return result;
}

} // end namespace om

std::istream& operator>>(std::istream& stream, om::u8_t&);

int main()
{
    using namespace om;
    using namespace std;

    cout << "Plus examples:\n";
    auto plus_arg_a0 = { 0b10101010, 0b00000001, 0b11111111,
                         0b11111111, 0b11111111, 0b11111111 };
    auto plus_arg_a1 = { 0b01010101, 0b00000011, 0b01111111,
                         0b10000000, 0b11111110, 0b11111111 };

    for (auto first = begin(plus_arg_a0), second = begin(plus_arg_a1);
         first != end(plus_arg_a0);
         ++first, ++second)
    {

        u8_t a0{ static_cast<byte>(*first) };
        u8_t a1{ static_cast<byte>(*second) };

        u8_t r0 = a0 + a1;

        cout << "-------------------------------------------\n";
        cout << "a0  == " << a0 << '(' << setw(3) << a0.to_str() << ")\n";
        cout << "a1  == " << a1 << '(' << setw(3) << a1.to_str() << ")\n";
        cout << "r0  == " << r0 << '(' << setw(3) << r0.to_str() << ")\n";

        int32_t     i_a0       = a0.to_int();
        int32_t     i_a1       = a1.to_int();
        int32_t     i_r0       = r0.to_int();
        const char* equal_sign = i_r0 == i_a0 + i_a1 ? " == " : " != ";
        cout << setw(3) << r0.to_int() << equal_sign << "(" << i_a0 << " + "
             << i_a1 << ")\n";
    }

    cout << "-------------------------------------------\n";
    cout << "Minus examples:\n";

    auto minus_arg_a0 = { 0b10101010, 0b00000001, 0b11111111,
                          0b11111111, 0b11111111, 0b11111111 };
    auto minus_arg_a1 = { 0b01010101, 0b00000011, 0b01111111,
                          0b10000000, 0b11111110, 0b11111111 };

    for (auto first = begin(minus_arg_a0), second = begin(minus_arg_a1);
         first != end(minus_arg_a0);
         ++first, ++second)
    {

        u8_t a0{ static_cast<byte>(*first) };
        u8_t a1{ static_cast<byte>(*second) };

        u8_t r0 = a0 - a1;
        cout << "-------------------------------------------\n";
        cout << "a0  == " << a0 << '(' << setw(3) << a0.to_str() << ")\n";
        cout << "a1  == " << a1 << '(' << setw(3) << a1.to_str() << ")\n";
        cout << "r0  == " << r0 << '(' << setw(3) << r0.to_str() << ")\n";

        int32_t     i_a0       = a0.to_int();
        int32_t     i_a1       = a1.to_int();
        int32_t     i_r0       = r0.to_int();
        const char* equal_sign = i_r0 == i_a0 - i_a1 ? " == " : " != ";
        cout << setw(3) << r0.to_int() << equal_sign << "(" << i_a0 << " - "
             << i_a1 << ")\n";
    }

    return 0;
}
