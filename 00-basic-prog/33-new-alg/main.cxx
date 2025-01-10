#include <iomanip>
// #include <generator> // for ultra cool colution not found on msvc compiler
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>

void some_algorithm_name(auto it_beg, auto it_end, auto it_out, auto func)
{
    for (; it_beg != it_end; it_beg++)
    {
        func(*it_beg, it_out);
    }
}
// std::generator<char> decodeCharacter(char ch)
// {
//     switch (ch)
//     {
//         case 'a': // duplicate
//         {
//             co_yield 'a';
//             co_yield 'a';
//             break;
//         }
//         case 'b':
//         {
//             break;
//         }
//         case 'c': // change/transform
//         {
//             co_yield 'd';
//             break;
//         }
//         case 'd': // change/transform
//         {
//             co_yield 'c';
//             break;
//         }
//         default: // copy
//         {
//             co_yield ch;
//         }
//     }
// }
// NOLINTNEXTLINE
int main(int argc, char** argv)
{
    using namespace std;
    string_view in = "abcdefaa";
    string      out;
    some_algorithm_name(begin(in),
                        end(in),
                        back_inserter(out),
                        [](char ch, auto& out_it)
                        {
                            switch (ch)
                            {
                                case 'a': // duplicate
                                {
                                    *out_it++ = 'a';
                                    *out_it++ = 'a';
                                    break;
                                }
                                case 'b': // skip
                                {
                                    break;
                                }
                                case 'c': // change/transform
                                {
                                    *out_it++ = 'd';
                                    break;
                                }
                                case 'd': // change/transform
                                {
                                    *out_it++ = 'c';
                                    break;
                                }
                                default: // copy
                                {
                                    *out_it++ = ch;
                                }
                            }
                        });

    string expected = "aadcefaaaa";
    cout << "input: " << quoted(in) << endl;
    cout << "output: " << quoted(expected) << (expected == out ? "==" : "!=")
         << quoted(out) << endl;

    string out2;
    auto   it_out = back_inserter(out2);

    std::ignore = accumulate(begin(in),
                             end(in),
                             it_out,
                             [](auto it_out, char ch)
                             {
                                 switch (ch)
                                 {
                                     case 'a': // duplicate
                                     {
                                         *it_out++ = 'a';
                                         *it_out++ = 'a';
                                         break;
                                     }
                                     case 'b': // skip
                                     {
                                         break;
                                     }
                                     case 'c': // change/transform
                                     {
                                         *it_out++ = 'd';
                                         break;
                                     }
                                     case 'd': // change/transform
                                     {
                                         *it_out++ = 'c';
                                         break;
                                     }
                                     default: // copy
                                     {
                                         *it_out++ = ch;
                                     }
                                 }
                                 return it_out;
                             });

    cout << "--- --- --- --- ---" << endl;
    cout << "using std::accumulate: " << quoted(out2) << endl;

    cout << "--- --- --- --- ---" << endl;

    // auto decode = [](std::string_view in) -> std::string
    // {
    //     using namespace std::views;
    //     return std::ranges::to<std::string>(in | transform(decodeCharacter) |
    //                                         join);
    // }

    return 0;
}
