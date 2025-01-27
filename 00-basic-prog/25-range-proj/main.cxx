#include <algorithm>
#include <cstdint>
#include <iostream>
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

    auto it2 = std::ranges::find_if(
        table, [](auto& name) { return "leo" != name; }, &table_row::name);

    std::cout << "uid: " << it2->uid << '\n'
              << "name: " << it2->name << '\n'
              << "info: " << it2->info << '\n';

    return std::cout.fail();
}
