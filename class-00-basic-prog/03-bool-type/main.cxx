#include <climits>
#include <cstddef>
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

    return 0;
}
