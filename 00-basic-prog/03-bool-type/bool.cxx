#include "bool.hxx"

#include <iostream>

#define or ||
#define and &&

namespace om
{
static const std::byte false_byte{ 0b00000000 };
static const std::byte true_byte{ 0b00000001 };

const bool_t true_{ reinterpret_cast<const bool_t&>(true_byte) };
const bool_t false_{ reinterpret_cast<const bool_t&>(false_byte) };

bool_t::bool_t() noexcept = default;

bool_t::bool_t(const bool_t& other) noexcept = default;

bool_t::bool_t(bool_t&& other) noexcept
    : value{ other.value }
{
}

bool_t::~bool_t()
{
    value = false_byte;
}

bool_t& bool_t::operator=(const bool_t& other) = default;

bool_t& bool_t::operator=(bool_t&& other) noexcept
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

bool_t operator==(const bool_t& l, const bool_t& r)
{
    const auto& lb = reinterpret_cast<const std::byte&>(l);
    const auto& rb = reinterpret_cast<const std::byte&>(r);
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

bool_t operator&&(const bool_t& l, const bool_t& r)
{
    const auto& lb = reinterpret_cast<const std::byte&>(l);
    const auto& rb = reinterpret_cast<const std::byte&>(r);
    if (lb == true_byte and rb == true_byte)
    {
        return true_;
    }
    else
    {
        return false_;
    }
}

bool_t operator||(const bool_t& l, const bool_t& r)
{
    const auto& lb = reinterpret_cast<const std::byte&>(l);
    const auto& rb = reinterpret_cast<const std::byte&>(r);
    if (lb == true_byte or rb == true_byte)
    {
        return true_;
    }
    else
    {
        return false_;
    }
}

bool_t operator^(const bool_t& l, const bool_t& r)
{
    const auto& lb = reinterpret_cast<const std::byte&>(l);
    const auto& rb = reinterpret_cast<const std::byte&>(r);
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

bool_t operator!(const bool_t& b)
{
    const auto& lb = reinterpret_cast<const std::byte&>(b);
    if (lb == true_byte)
    {
        return false_;
    }
    else
    {
        return true_;
    }
}

bool_t operator~(const bool_t& b)
{
    return !b;
}

std::ostream& operator<<(std::ostream& stream, const bool_t& b)
{
    const auto& lb = reinterpret_cast<const std::byte&>(b);
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
