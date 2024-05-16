#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdfloat> //std::float16_t from c++23
#include <string>

struct float_bits
{
    float value;
};

std::string no_tail_zero(double value)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(50);
    oss << value;
    std::string result = oss.str();
    auto        it     = std::adjacent_find(
        result.rbegin(), result.rend(), std::not_equal_to<char>());
    if (*it == '0')
    {
        // drop last zeroes like: 0.123000000 -> 0.123
        size_t num_of_zeroes = std::distance(result.rbegin(), it);
        size_t size          = result.size();
        result.resize(size - num_of_zeroes);
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const float_bits& value)
{
    // online visualizer: https://www.h-schmidt.net/FloatConverter/IEEE754.html
    // TODO implement minimal, maximum, NaN, Inf, denormalized
    using namespace std;
    out << "sEEEEEEEEfffffffffffffffffffffff" << endl;
    uint32_t as_i32   = *reinterpret_cast<const uint32_t*>(&value.value);
    uint32_t sign_bit = as_i32 & 0x80'00'00'00; // 1 hi bit
    uint32_t exp      = as_i32 & 0x7F'80'00'00; // 8 hi bit after "sign"
    uint32_t fraction = as_i32 & 0x00'7F'FF'FF; // 23 hi bits after exp
    sign_bit >>= 31;
    bitset<1> sign_bits(sign_bit);
    out << sign_bits << string(31, '_') << " sign 1 bit 0->(+) 1->(-)" << endl;
    exp >>= 23;
    bitset<8> exp_bits(exp);
    int32_t   real_exp    = exp - 127;
    double    two_pow_exp = pow(2.0, real_exp);
    out << fixed << setprecision(30);
    out << '_' << exp_bits << string(23, '_') << " exp=(x-127) " << exp
        << "-127=" << real_exp << " 2^" << real_exp << "="
        << no_tail_zero(two_pow_exp) << endl;
    bitset<23> fraction_bits(fraction);
    double     fraction_value = .0;
    for (int32_t i = 23; i >= 1; --i)
    {
        if (fraction_bits[23 - i])
        {
            // 1/2 1/4 1/8 1/16 ... 1/(2^23)
            double next_fraction = pow(2.0, -i);
            fraction_value += next_fraction;
        }
    }
    out << string(9, '_') << fraction_bits << " fraction 23 bits " << fixed
        << no_tail_zero(fraction_value) << endl;
    const char* sign_char = sign_bit ? "-" : "";
    out << "s*exp*(1+fraction)=" << sign_char << no_tail_zero(two_pow_exp)
        << "*(1.0+" << no_tail_zero(fraction_value) << ")=" << sign_char
        << no_tail_zero(two_pow_exp * (1.0 + fraction_value)) << endl;
    return out;
}

int main(int argc, char** argv)
{
    std::float16_t  f16_support{};  // just checking
    std::float32_t  f32_support{};  // just checking
    std::float64_t  f64_support{};  // just checking
    std::float128_t f128_support{}; // just checking

    if (argc > 1 && argv[1] == std::string("-i"))
    {
        float user_value{};
        std::cout << "type float value (or 0 for exit):" << std::endl;
        std::cin >> user_value;
        while (user_value != 0.f)
        {
            std::cout << float_bits{ user_value };
            std::cin >> user_value;
        }
    }
    else
    {
        const std::initializer_list<float> intresting_floats{
            0.f,
            1.f,
            0.1f,
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max() +
                std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min() / 2.0f
        };
        for (float value : intresting_floats)
        {
            std::cout << "float " << no_tail_zero(value) << " :" << std::endl
                      << float_bits{ value };
        }
    }
    return std::cout.fail();
}
