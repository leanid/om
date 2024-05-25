#include "files.hxx"

#include <fstream>

namespace om::files
{

content_t read_file(const std::filesystem::path& path)
{
    content_t out;

    std::ifstream f;
    f.exceptions(std::ios::badbit | std::ios::failbit);
    f.open(path, std::ios_base::binary);

    f.seekg(0, std::ios_base::end);
    size_t size = f.tellg();
    f.seekg(0, std::ios_base::beg);

    out.memory = std::make_unique<std::byte[]>(size);

    f.read(reinterpret_cast<char*>(out.memory.get()), size);
    out.size = size;

    return out;
}

std::string_view content_t::as_string_view() const
{
    return { reinterpret_cast<char*>(memory.get()), size };
}
} // namespace om::files
