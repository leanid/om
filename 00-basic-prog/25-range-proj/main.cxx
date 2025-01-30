#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

struct table_row
{
    std::uint32_t uid;
    std::string   name;
    std::string   info;
};
// NOLINTNEXTLINE
int main(int argc, char** argv)
{
    const std::vector<table_row> table = {
        { .uid = 1, .name = "leo", .info = "developer" },
        { .uid = 2, .name = "dima", .info = "developer" },
        { .uid = 3, .name = "igor", .info = "developer" }
    };

    auto it = std::ranges::find(table, "igor", &table_row::name);

    std::cout << "uid: " << it->uid << '\n'
              << "name: " << it->name << '\n'
              << "info: " << it->info << '\n';

    auto it2 = std::ranges::find_if_not(
        table, [](auto& name) { return "leo" == name; }, &table_row::name);

    std::cout << "uid: " << it2->uid << '\n'
              << "name: " << it2->name << '\n'
              << "info: " << it2->info << '\n';

    auto fn_name0 = []() -> std::string { return ""; };
    auto fn_name1 = []() -> std::string { return "leo"; };
    auto fn_name2 = []() -> std::string { return "dima"; };
    auto fn_name3 = []() -> std::string { return "igor"; };

    std::array<std::string (*)(), 4> arr{
        fn_name0, fn_name1, fn_name2, fn_name3
    };

    auto not_empty = [](const std::string& str) { return !str.empty(); };
    auto get_str   = [](auto fn) -> std::string { return fn(); };

    auto first_not_empty_str = arr | std::views::transform(get_str) |
                               std::views::filter(not_empty) |
                               std::views::take(1);

    if (first_not_empty_str.begin() != first_not_empty_str.end())
    {
        std::cout << "found first not empty string: ["
                  << *first_not_empty_str.begin() << "]" << std::endl;
    }

    return std::cout.fail();
}
