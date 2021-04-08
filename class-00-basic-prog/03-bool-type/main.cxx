#include <iostream>

#include "bool.hxx"

int main(int, char**)
{
    using namespace om;

    // clang-format off
    std::cout << "/// operation | first value | second value | result\n";
    std::cout << "/// -----------------------------------------------\n";
    std::cout << "///     ==    |    true     |   true       | " << (true_ == true_) << "\n";
    std::cout << "///     ==    |    true     |   false      | " << (true_ == false_) << "\n";
    std::cout << "///     ==    |    false    |   true       | " << (false_ == true_) << "\n";
    std::cout << "///     ==    |    false    |   false      | " << (false_ == false_) << "\n";
    std::cout << "/// -----------------------------------------------\n";
    std::cout << "///     &&    |    true     |   true       | " << (true_ && true_) << "\n";
    std::cout << "///     &&    |    true     |   false      | " << (true_ && false_) << "\n";
    std::cout << "///     &&    |    false    |   true       | " << (false_ && true_) << "\n";
    std::cout << "///     &&    |    false    |   false      | " << (false_ && false_) << "\n";
    std::cout << "/// -----------------------------------------------\n";
    std::cout << "///     ||    |    true     |   true       | " << (true_ || true_) << "\n";
    std::cout << "///     ||    |    true     |   false      | " << (true_ || false_) << "\n";
    std::cout << "///     ||    |    false    |   true       | " << (false_ || true_) << "\n";
    std::cout << "///     ||    |    false    |   false      | " << (false_ || false_) << "\n";
    std::cout << "/// -----------------------------------------------\n";
    std::cout << "///     ^     |    true     |   true       | " << (true_ ^ true_) << "\n";
    std::cout << "///     ^     |    true     |   false      | " << (true_ ^ false_) << "\n";
    std::cout << "///     ^     |    false    |   true       | " << (false_ ^ true_) << "\n";
    std::cout << "///     ^     |    false    |   false      | " << (false_ ^ false_) << "\n";
    std::cout << "/// -----------------------------------------------\n";
    std::cout << "///     !     |    true     |              | " << (!true_) << "\n";
    std::cout << "///     !     |    false    |              | " << (!false_) << "\n";
    std::cout << "/// -----------------------------------------------\n";
    // clang-format on;

    om::bool_t b0;
    om::bool_t b1{false_};
    om::bool_t b2{true_};
    om::bool_t b3{b1 ^ b2};

    bool cpp_lang_boolean = static_cast<bool>(b3);

    std::cout << "b0 == " << b0 << '\n';
    std::cout << "b1 == " << b1 << '\n';
    std::cout << "b2 == " << b2 << '\n';
    std::cout << "b3 == " << b3 << '\n';
    std::cout << "cpp_lang_boolean == " << std::boolalpha << cpp_lang_boolean << '\n';

    return 0;
}
