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

int main(int argc, char** argv)
{
    std::vector<table_row> table = { { 1, "leo", "developer" },
                                     { 2, "dima", "developer" },
                                     { 3, "igor", "developer" } };

    auto it = std::ranges::find(table, "igor", &table_row::name);

    std::cout << "uid: " << it->uid << '\n'
              << "name: " << it->name << '\n'
              << "info: " << it->info << '\n';
    return std::cout.fail();
}
