#include "files.hxx"

#include <fstream>

namespace om::files
{

content_t read_file(const std::filesystem::path& path)
{
    using namespace std;
    content_t out;

    ifstream f;
    f.exceptions(ios::badbit | ios::failbit);
    f.open(path, ios::binary | ios::ate);

    ifstream::pos_type size         = f.tellg();
    std::streamoff     size_integer = size;

    out.memory = make_unique<byte[]>(size_integer);

    f.seekg(0, ios_base::beg);
    f.read(reinterpret_cast<char*>(out.memory.get()), size_integer);
    out.size = size;

    return out;
}

std::string_view content_t::as_string_view() const
{
    return { reinterpret_cast<char*>(memory.get()), size };
}
} // namespace om::files
