#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

std::stringstream filter_comments(std::string_view file)
{
    std::stringstream out;
    std::string       line;
    std::ifstream     in(file.data(), std::ios_base::binary);

    while (std::getline(in, line))
    {
        size_t comment_pos = line.find("//");
        if (comment_pos != std::string::npos)
        {
            line = line.substr(0, comment_pos);
        }
        if (!line.empty())
        {
            out << line << '\n';
        }
    }

    return out;
}
