#pragma once

#include <cstddef>

namespace om
{
/// from standard:
/// bool - type, capable of holding one of the two values: true or false.
/// The value of sizeof(bool) is implementation defined and might differ from 1.
class bool_
{
public:
    bool_()                   = default;
    bool_(const bool_& other) = default;
    bool_(bool_&&)            = default;
    bool_& operator=(const bool_&) = default;
    bool_& operator=(bool_&&) = default;

    friend bool_ operator&&(const bool_& l, const bool_& r);
    friend bool_ operator||(const bool_& l, const bool_& r);
    bool_        operator!();
};

extern const bool_ true_;
extern const bool_ false_;

/// operation | first value | second value | result
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

} // end namespace om
