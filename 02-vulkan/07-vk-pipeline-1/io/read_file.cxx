#include "read_file.hxx"

#include <exception>
#include <fstream>
#include <stdexcept>

namespace om::io
{

content read_file(const std::filesystem::path& path)
{
    using namespace std;
    content out;

    ifstream f;
    f.exceptions(ios::badbit | ios::failbit);

    try
    {
        f.open(path, ios::binary | ios::ate);

        ifstream::pos_type size         = f.tellg();
        streamoff          size_integer = size;

        out.memory = make_unique<byte[]>(size_integer);

        f.seekg(0, ios_base::beg);
        f.read(reinterpret_cast<char*>(out.memory.get()), size_integer);
        out.size = size_integer;
    }
    catch (const std::exception& e)
    {
        string msg;
        msg.reserve(512);
        msg += "error: can't read file [";
        msg += path.generic_string();
        msg += "] cause: ";
        msg += e.what();
        throw runtime_error(msg);
    }

    return out;
}

} // namespace om::io
