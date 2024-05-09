#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

int main(int argc, char** argv)
{
    using namespace std;

    // we use only c++ io library
    std::ios::sync_with_stdio(false);

    {
        // prepare file
        ofstream test_file("test_file");
        test_file << "\n some file test text \n\n";
    }

    {
        // copy file naive way
        ifstream in("test_file");
        ofstream out("test_file_copy");
        // out << in; // not compile
        out << in.rdbuf();
    }

    {
        filebuf in;
        if (!in.open("/home/leo/Videos/video_2024-05-05_12-44-07.mp4",
                     ios_base::binary | ios_base::in))
        {
            cout << "error: can't open test_file" << endl;
            return 1;
        }
        streamsize availiable_size = in.in_avail();
        cout << "in_avail(): " << availiable_size << endl; // -1
        std::streampos pos = in.pubseekoff(0, ios_base::end, ios_base::in);
        if (pos == std::streampos(-1))
        {
            cout << "error: can't get EOF position" << endl;
            return 1;
        }

        uint64_t file_size = pos;
        cout << "file_size: " << file_size << " in_avail: " << availiable_size
             << endl;
        in.pubseekoff(0, ios_base::beg, ios_base::in);
        unique_ptr<char[]> content = make_unique<char[]>(file_size);
        streamsize         size    = file_size;
        streamsize         copied  = in.sgetn(content.get(), file_size);

        if (copied != file_size)
        {
            cout << "error: can't read full file" << endl;
            return 1;
        }

        cout << "file fully loaded: " << string_view(content.get(), 4) << endl;
        // try to drop cache and see free memory status (all from root)
        // free -m  && sync && echo 3 > /proc/sys/vm/drop_caches && free -m
    }
    return 0;
}
