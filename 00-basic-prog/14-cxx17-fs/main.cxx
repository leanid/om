#include <filesystem>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    namespace fs = std::filesystem;

    const fs::path cwd = fs::current_path();

    std::cout << "cwd: " << cwd << std::endl;

    for (auto const& dir_entry : fs::recursive_directory_iterator{ cwd })
    {
        std::cout << dir_entry.path().u8string() << '\n';
    }
    return 0;
}
