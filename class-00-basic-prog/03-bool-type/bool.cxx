#include "bool.hxx"

#include <iostream>

namespace om
{
static const std::byte false_byte{ 0b00000000 };
static const std::byte true_byte{ 0b00000001 };

const bool_ true_{ reinterpret_cast<const bool_&>(true_byte) };
const bool_ false_{ reinterpret_cast<const bool_&>(false_byte) };

bool_::bool_()
    : value{ false_byte } {};

bool_::bool_(const bool_& other)
    : value{ other.value }
{
}

bool_::bool_(bool_&& other)
    : value{ other.value }
{
}

bool_::~bool_()
{
    value = false_byte;
}

bool_& bool_::operator=(const bool_& other)
{
    value = other.value;
    return *this;
}

bool_& bool_::operator=(bool_&& other)
{
    value = other.value;
    return *this;
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

bool_ operator==(bool_ l, bool_ r)
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

bool_ operator&&(bool_ l, bool_ r)
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

bool_ operator||(bool_ l, bool_ r)
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

bool_ operator^(bool_ l, bool_ r)
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

bool_ operator!(bool_ b)
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

bool_ operator~(bool_ b)
{
    return !b;
}

std::ostream& operator<<(std::ostream& stream, const bool_& b)
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
std::istream& operator>>(std::istream& stream, bool_& result)
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
