#include "bool.hxx"

#include <iostream>

namespace om
{
static const std::byte false_byte{ 0b00000000 };
static const std::byte true_byte{ 0b00000001 };

const bool_t true_{ reinterpret_cast<const bool_t&>(true_byte) };
const bool_t false_{ reinterpret_cast<const bool_t&>(false_byte) };

bool_t::bool_t()
    : value{ false_byte } {};

bool_t::bool_t(const bool_t& other)
    : value{ other.value }
{
}

bool_t::bool_t(bool_t&& other)
    : value{ other.value }
{
}

bool_t::~bool_t()
{
    value = false_byte;
}

bool_t& bool_t::operator=(const bool_t& other)
{
    value = other.value;
    return *this;
}

bool_t& bool_t::operator=(bool_t&& other)
{
    value = other.value;
    return *this;
}

bool_t::operator bool() const
{
    return static_cast<bool>(value & true_byte);
}

/// operation | first value | second value | result
/// -----------------------------------------------
///     ==    |    true     |   true       | true
///     ==    |    true     |   false      | false
///     ==    |    false    |   true       | false
///     ==    |    false    |   false      | true
/// -----------------------------------------------
///     &&    |    true     |   true       | true
///     &&    |    true     |   false      | false
///     &&    |    false    |   true       | false
///     &&    |    false    |   false      | false
/// -----------------------------------------------
///     ||    |    true     |   true       | true
///     ||    |    true     |   false      | true
///     ||    |    false    |   true       | true
///     ||    |    false    |   false      | false
/// -----------------------------------------------
///     ^     |    true     |   true       | false
///     ^     |    true     |   false      | true
///     ^     |    false    |   true       | true
///     ^     |    false    |   false      | false
/// -----------------------------------------------
///     !     |    true     |              | false
///     !     |    false    |              | true
/// -----------------------------------------------

bool_t operator==(bool_t l, bool_t r)
{
    const std::byte& lb = reinterpret_cast<const std::byte&>(l);
    const std::byte& rb = reinterpret_cast<const std::byte&>(r);
    if ((lb == true_byte and rb == true_byte) or
        (lb == false_byte && rb == false_byte))
    {
        return true_;
    }
    else
    {
        return false_;
    }
}

bool_t operator&&(bool_t l, bool_t r)
{
    const std::byte& lb = reinterpret_cast<const std::byte&>(l);
    const std::byte& rb = reinterpret_cast<const std::byte&>(r);
    if (lb == true_byte and rb == true_byte)
    {
        return true_;
    }
    else
    {
        return false_;
    }
}

bool_t operator||(bool_t l, bool_t r)
{
    const std::byte& lb = reinterpret_cast<const std::byte&>(l);
    const std::byte& rb = reinterpret_cast<const std::byte&>(r);
    if (lb == true_byte or rb == true_byte)
    {
        return true_;
    }
    else
    {
        return false_;
    }
}

bool_t operator^(bool_t l, bool_t r)
{
    const std::byte& lb = reinterpret_cast<const std::byte&>(l);
    const std::byte& rb = reinterpret_cast<const std::byte&>(r);
    if ((lb == true_byte and rb == false_byte) or
        (lb == false_byte and rb == true_byte))
    {
        return true_;
    }
    else
    {
        return false_;
    }
}

bool_t operator!(bool_t b)
{
    const std::byte& lb = reinterpret_cast<const std::byte&>(b);
    if (lb == true_byte)
    {
        return false_;
    }
    else
    {
        return true_;
    }
}

bool_t operator~(bool_t b)
{
    return !b;
}

std::ostream& operator<<(std::ostream& stream, const bool_t& b)
{
    const std::byte& lb = reinterpret_cast<const std::byte&>(b);
    if (lb == true_byte)
    {
        stream << "true";
    }
    else
    {
        stream << "false";
    }
    return stream;
}
std::istream& operator>>(std::istream& stream, bool_t& result)
{
    std::string str;
    stream >> str;
    if (str == "true")
    {
        result = true_;
    }
    else if (str == "false")
    {
        result = false_;
    }
    else
    {
        stream.setstate(std::ios_base::failbit);
    }
    return stream;
}

} // namespace om

/// 4-bit unsigned int table     | school example:|
/// number  | bits   |           | 7        0b0111|
/// --------+--------+           |+5       +0b0101|
///  0      | 0b0000 |           |__        ______|
///  1      | 0b0001 |           |12        0b1100|
///  2      | 0b0010 |           +----------------+
///  3      | 0b0011 |
///  4      | 0b0100 |   Выбор как закодировать эти
///  5      | 0b0101 |   числа не случаен. Все дело
///  6      | 0b0110 |   в позиционной системе исчисления.
///  7      | 0b0111 |
///  8      | 0b1000 |   пример: позиция 1 означает 2^index
///  9      | 0b1001 |   0b1111 = 0b1000 = 2*2*2 (2^3) = 8
/// 10      | 0b1010 |           +0b0100 = 2*2   (2^2) = 4
/// 11      | 0b1011 |           +0b0010 = 2     (2^1) = 2
/// 12      | 0b1100 |           +0b0001 = 1     (2^0) = 1
/// 13      | 0b1101 |           -------------------------
/// 14      | 0b1110 |            0b1111                15
/// 15      | 0b1111 |
