#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

void some_algorithm_name(auto it_beg, auto it_end, auto it_out, auto func)
{
    for (; it_beg != it_end; it_beg++)
    {
        func(*it_beg, it_out);
    }
}

int main(int argc, char** argv)
{
    using namespace std;
    string_view in = "abcdef";
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

    string expected = "aadcef";
    cout << quoted(expected) << (expected == out ? "==" : "!=") << quoted(out)
         << endl;
    return 0;
}
