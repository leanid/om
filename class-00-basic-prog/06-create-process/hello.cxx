#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << "hello world" << std::flush;
  return std::cout.fail();
}
