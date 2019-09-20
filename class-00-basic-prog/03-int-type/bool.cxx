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

/// +---+---+---+---+
/// |0/1|0/1|0/1|0/1|
/// +---+---+---+---+
///   3   2   1   0   <- индекс позиции
///                     (или степерь в которую возводим
///                      основание системы счисления)


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
///
///


///
///  7 в десятичной системе счисления.
/// +---+---+---+---+
/// |  0|  1|  1|  1|
/// +---+---+---+---+
///   3   2   1   0        <- индекс позиции
///               2^0=1    (или степерь в которую возводим
///           2^1=2         основание системы счисления)
///       2^2=4
///   2^3=8
///
///       4 + 2 + 1 = 7
///
///
///
///
///
/// 4-bit unsigned int and signed int
/// number  | bits   | number |
/// --------+--------+--------+
///  0      | 0b0000 |  0     |
///  1      | 0b0001 |  1     |
///  2      | 0b0010 |  2     |
///  3      | 0b0011 |  3     |
///  4      | 0b0100 |  4     |
///  5      | 0b0101 |  5     |
///  6      | 0b0110 |  6     |
///  7      | 0b0111 |  7     |
///  8      | 0b1000 | -8     |
///  9      | 0b1001 | -7     |
/// 10      | 0b1010 | -6     |
/// 11      | 0b1011 | -5     |
/// 12      | 0b1100 | -4     |
/// 13      | 0b1101 | -3     |
/// 14      | 0b1110 | -2     |
/// 15      | 0b1111 | -1     |
///
///
/// Добавление 0b0001 - добавление 1 к нашему числу
/// Добавление 0b1111 - вычитание 1 от нашего числа
/// Так происходит, т.к. все числа замкнуты и упорядочены
/// в "кольцо". Что бы отнять, можно просто добавить,
/// "дополнение" до кольца, для нашего числа.
///
/// Отнимим 7 от 12 например: 12 - 7 = 5
///                     0b1100 -  0b0111 = 0b0101
///                     0b1100 +(-0b0111)= 0b0101
///                     0b1100 +(+0b1001)= 0b0101
/// "дополнение" - это то, что надо добавить,
/// что бы получить "кольцо" - 16 - в нашем случае
/// в двоичной стистеме, читается как:
/// 1. инвертирование всех битиков ~ (0b0111->0b1000)
/// 2. добавление 1 (0b1000->0b1001)
///
///
/// Fixed-Point numbers:
/// 0bXXXX.XXXX - X = {0/1}
///   ^^^^ ^^^^
///   |||| ||||
///   |||| |||+---- 2^(-4) = 1/2*2*2*2 = 1/16 = 0.0625
///   |||| ||+----- 2^(-3) = 1/2*2*2   = 1/8  = 0.125
///   |||| |+------ 2^(-2) = 1/2*2     = 1/4  = 0.25
///   |||| +------- 2^(-1) = 1/2       = 1/2  = 0.5
///   |||+--------- 2^(0)  = 1         = 1    = 1
///   ||+---------- 2^(1)  = 2         = 2    = 2
///   |+----------- 2^(2)  = 2*2       = 4    = 4
///   +------------ 2^(3)  = 2*2*2     = 8    = 8
///
/// Например: 0b1001.1001 = означает:
///            +8
///               +1
///                 +0.5
///                   +0.0625
///           9.5625 - наше число
///
///        FLOAT 32 bit
///    0b0|01111100|01000000000000000000000
///      ^+--------+----------------------+
/// sign 1 ^^^^^^^^
/// exponenta 8     ^^^^^^^^^^^^^^^^^^^^^^
///                   fractions (22 bits)
///
/// sign = 1/0 (1 - отицательное, 0 - полож.)
/// exponenta - степень, двойки, минус -127(0b10000001)
/// exponenta = 0b01111100
///            +0b10000001
///            -----------
///             0b11111101 == -3 т.е. 2^(-3) == 1/2*2*2 = 1/8 = 0.125
/// т.е. теперь надо узнать какие доли от 0.125 нужно добавить к этому
/// числу. Видим что у нас там второй битик в 1,
/// это значит что 1/4 (т.е. 2^(-2)) == 0.25 - эту долю мы должны взять
/// от экспоненты 0.125 * 0.25 = 0.03125
/// Результат: сложим экспоненту и все ее доли: 0.125 + 0.031125 = 0.15625
///
///
///
///
///
///
