/// define CATCH_AMALGAMATED_CUSTOM_MAIN=1
/// if you want to write your own int main(...)
/// @code
/// int main( int argc, char* argv[] ) {
///  // your setup ...
///
///  int result = Catch::Session().run( argc, argv );
///
///  // your clean-up...
///
///  return result;
/// }
/// @endcode
#include "catch_amalgamated.hpp"

int factorial(int number)
{
    return number <= 1 ? number : factorial(number - 1) * number;
}
// NOLINTBEGIN(*)
TEST_CASE("testing the factorial function")
{
    CHECK(factorial(0) == 1);
    CHECK(factorial(1) == 1);
    CHECK(factorial(2) == 2);
    CHECK(factorial(3) == 6);
    CHECK(factorial(10) == 3628800);
}

TEST_CASE("vectors can be sized and resized")
{
    std::vector<int> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    SECTION("adding to the vector increases its size")
    {
        v.push_back(1);

        CHECK(v.size() == 6);
        CHECK(v.capacity() >= 6);
    }
    SECTION("reserving increases just the capacity")
    {
        v.reserve(6);

        CHECK(v.size() == 5);
        CHECK(v.capacity() >= 6);
    }
}
#include <iostream>
using namespace std;
// every SECTION - start from the begining of TEST_CASE visualize it:
TEST_CASE("lots of nested subcases")
{
    cout << endl << "root" << endl;
    SECTION("")
    {
        cout << "1" << endl;
        SECTION("")
        {
            cout << "1.1" << endl;
        }
    }
    SECTION("")
    {
        cout << "2" << endl;
        SECTION("")
        {
            cout << "2.1" << endl;
        }
        SECTION("")
        {
            cout << "2.2" << endl;
            SECTION("")
            {
                cout << "2.2.1" << endl;
                SECTION("")
                {
                    cout << "2.2.1.1" << endl;
                }
                SECTION("")
                {
                    cout << "2.2.1.2" << endl;
                }
            }
        }
        SECTION("")
        {
            cout << "2.3" << endl;
        }
        SECTION("")
        {
            cout << "2.4" << endl;
        }
    }
}

// NOLINTEND(*)
