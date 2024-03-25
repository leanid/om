#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace om
{
struct table_row
{
    std::uint32_t uid;
    std::string   name;
    std::string   info;
};
} // namespace om

int main(int argc, char** argv)
{
    using namespace std;

    vector<om::table_row> table = { { 1, "leo", "developer" },
                                    { 2, "dima", "developer" },
                                    { 3, "igor", "developer" } };

    auto it = ranges::find(table, "igor", &om::table_row::name);

    cout << "uid: " << it->uid << '\n'
         << "name: " << it->name << '\n'
         << "info: " << it->info << '\n';
    return cout.fail();
}
