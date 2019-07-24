#include <climits>
#include <cstddef>
#include <iostream>

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
    u8_t(std::byte v)
        : value{ v }
    {
    }

    std::string to_str() const;

    friend u8_t operator+(const u8_t& l, const u8_t& r);
    friend u8_t operator-(const u8_t& l, const u8_t& r);
    friend u8_t operator<(const u8_t& l, const u8_t& r);

private:
    friend std::ostream& operator<<(std::ostream& stream, const u8_t&);
    friend std::istream& operator>>(std::istream& stream, u8_t&);

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

    return result;
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

} // end namespace om

std::istream& operator>>(std::istream& stream, om::u8_t&);

int main()
{
    using namespace om;

    auto first_operands  = { 0b10101010, 0b00000001, 0b11111111 };
    auto second_operands = { 0b01010101, 0b00000001, 0b01111111 };

    for (auto first  = std::begin(first_operands),
              second = std::begin(second_operands);
         first != std::end(first_operands); ++first, ++second)
    {

        u8_t a0{ static_cast<std::byte>(*first) };
        u8_t a1{ static_cast<std::byte>(*second) };

        u8_t r0 = a0 + a1;

        std::cout << "a0 == " << a0 << '(' << a0.to_str() << ")\n";
        std::cout << "a1 == " << a1 << '(' << a1.to_str() << ")\n";
        std::cout << "r0 == " << r0 << '(' << r0.to_str() << ")\n";
    }

    return 0;
}
