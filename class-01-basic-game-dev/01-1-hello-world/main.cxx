#include <cstdlib>
#include <iostream>
#include <string_view>

int main(int /*argc*/, char* /*argv*/ [])
{
    using namespace std;

    string_view output_phrase("hello world");

    cout << output_phrase << endl;

    bool is_good = cout.good();

    int result = is_good ? EXIT_SUCCESS : EXIT_FAILURE;
    return result;
}
