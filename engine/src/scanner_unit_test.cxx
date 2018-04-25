#include "scanner.hxx"

int main()
{
    // TODO implement unit test for scanner class
    // using https://github.com/catchorg/Catch2

    om::scanner scanner;

    om::directory_info dir_info = scanner.scan("assets");

    om::file_info file_info = dir_info.tree.get_file_info("file.txt");

    file_info.exist == true;
    file_info.size == ("Hello World"s).size();

    return EXIT_FAILURE;
}
