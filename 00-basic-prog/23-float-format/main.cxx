#include <bitset>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

struct float_bits
{
    float value;
};

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
        << "-127=" << real_exp << " 2^" << real_exp << "=" << two_pow_exp
        << endl;
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
        << fraction_value << endl;
    const char* sign_char = sign_bit ? "-" : "";
    out << "s*exp*(1+fraction)=" << sign_char << two_pow_exp << "*(1.0+"
        << fraction_value << ")=" << sign_char
        << two_pow_exp * (1.0 + fraction_value) << endl;
    return out;
}

int main(int argc, char** argv)
{
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
        std::cout << float_bits{ 4.f };
    }
    return std::cout.fail();
}
