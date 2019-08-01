#include <cstdlib>

int main(int argc, char* argv[])
{
  for(size_t i = 0; i < 1024; ++i)
    {
      int result = std::system("./hello");
      if (result != 0)
        {
          return result;
        }
    }
  return 0;
}
