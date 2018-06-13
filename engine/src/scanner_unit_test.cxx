#include "scanner.hxx"

int main()
{
    // TODO implement unit test for scanner class
    // using https://github.com/catchorg/Catch2

    om::scanner scanner;
    scanner.init("C:/Users/codelimit/git/om");
    scanner.scan();
    scanner.tell_directory_info("jni");
    scanner.tell_directory_info("04-0-render-basic");

    return EXIT_FAILURE;
}
