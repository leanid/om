#include <iostream>
#include <system_error>

int main(int argc, char** argv)
{
    std::cout << "int value | ec.category | ec.message | ec.condition.category "
                 "| ec.condition.message |"
              << std::endl;

    for (unsigned i = 0; i < 255u; i++)
    {
        auto            error              = static_cast<std::errc>(i);
        std::error_code ec                 = std::make_error_code(error);
        auto            msg                = ec.message();
        auto            category           = ec.category().name();
        auto            condition          = ec.default_error_condition();
        const auto&     condition_category = condition.category();
        auto            category2          = condition_category.name();
        auto            msg2               = condition.message();
        std::cout << i << ": " << category << ' ' << msg << " " << category2
                  << " " << msg2 << std::endl;
    }

    return std::cout.fail();
}
