#pragma once

#include <cstddef>
#include <iosfwd>

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

namespace om
{
/// from standard:
/// bool - type, capable of holding one of the two values: true or false.
/// The value of sizeof(bool) is implementation defined and might differ from 1.
struct bool_t
{
    bool_t();
    bool_t(const bool_t& other);
    bool_t(bool_t&&) noexcept;
    ~bool_t();
    bool_t& operator=(const bool_t&);
    bool_t& operator=(bool_t&&) noexcept;

    explicit operator bool() const;

    friend std::ostream& operator<<(std::ostream& stream, const bool_t&);
    friend std::istream& operator>>(std::istream& stream, bool_t&);

private:
    std::byte value;
};

bool_t operator==(bool_t l, bool_t r);
bool_t operator&&(bool_t l, bool_t r);
bool_t operator||(bool_t l, bool_t r);
bool_t operator^(bool_t l, bool_t r);
bool_t operator!(bool_t b);
bool_t operator~(bool_t b);

extern const bool_t true_;
extern const bool_t false_;

} // end namespace om
