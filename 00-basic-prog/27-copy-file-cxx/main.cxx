#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    using namespace std;

    {
        // prepare file
        std::ofstream test_file("test_file");
        test_file << "\n some file test text \n\n";
    }

    {
        // copy file naive way
        std::ifstream in("test_file");
        std::ofstream out("test_file_copy");
        // out << in; // not compile
    }

    {
        std::filebuf in;
        in.open("test_file", std::ios_base::binary);
        std::cout << "in_avail(): " << in.in_avail() << std::endl;
    }
    return 0;
}
