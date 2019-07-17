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
struct bool_
{
    bool_();
    bool_(const bool_& other);
    bool_(bool_&&);
    ~bool_();
    bool_& operator=(const bool_&);
    bool_& operator=(bool_&&);

    friend std::ostream& operator<<(std::ostream& stream, const bool_&);
    friend std::ostream& operator>>(std::ostream& stream, bool_&);

private:
    std::byte value;
};

bool_ operator==(bool_ l, bool_ r);
bool_ operator&&(bool_ l, bool_ r);
bool_ operator||(bool_ l, bool_ r);
bool_ operator^(bool_ l, bool_ r);
bool_ operator!(bool_ b);
bool_ operator~(bool_ b);

extern const bool_ true_;
extern const bool_ false_;

} // end namespace om
