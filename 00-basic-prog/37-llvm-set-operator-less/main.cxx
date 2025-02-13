#include <cstdlib>
#include <functional>
#include <iostream>
#include <set>

struct my_type
{
    bool operator<(const my_type& other) const { return true; }
};

int main()
{
    std::set<my_type> s01;
    std::set<my_type> s02;
    std::cout << "(s01 < s02): " << (s01 < s02) << std::endl; // compiles

    std::set<my_type, std::less<my_type>> s11;
    std::set<my_type, std::less<my_type>> s12;

    std::cout << "(s11 < s12): " << (s11 < s12) << std::endl; // compiles

    std::set<my_type, std::less<my_type>, std::allocator<my_type>> s21;
    std::set<my_type, std::less<my_type>, std::allocator<my_type>> s22;

    std::cout << "(s21 < s22): " << (s21 < s22) << std::endl; // error
    return EXIT_SUCCESS;
}
