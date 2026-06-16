#include <cstdlib>
#include <print>

int main()
{
    // по умолчанию std::println пишет в С-шный поток stdout
    std::println(
        "main_print_std23.cxx hello world form c++23 #include <print>");
    // std::fflush сбрасывает буфер на уровень ОС.
    // Если ОС (например, драйвер /dev/video0) вернет ошибку, fflush вернет
    // EOF. Также ferror(stdout) проверит, не выставлялся ли флаг ошибки
    // потока ранее.
    if (std::fflush(stdout) == EOF || std::ferror(stdout))
    {
        // Выводим системную ошибку в поток std::stderr (так как stdout
        // сломан)
        std::perror("Error writing to stdout");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
