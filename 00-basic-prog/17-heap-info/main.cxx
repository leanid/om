#include "mem_info.hxx"

#include <fstream>
#include <iomanip>
#include <iostream>

void print(const om::heap_info::info& heap_info)
{
    using namespace std;
    cout << "[heap] " << heap_info << " "
         << ((heap_info.max - heap_info.min) / (1024.0 * 1024.0)) << "MB"
         << endl;
}

void dump_mem_info(const char* file_name)
{
    using namespace std;
    ofstream out(file_name, ios::binary);
    ifstream in("/proc/self/maps", ios::binary);
    out << in.rdbuf();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    using namespace std;

    dump_mem_info(".mem_info_start");

    auto heap_info = om::heap_info::get_heap_process_segment_layout();

    print(heap_info);

    string str;
    str.resize(1024 * 1024 * 100); // 100MB

    auto space_index = str.find(' ');
    if (space_index != string::npos)
    {
        cout << "error: found space in empty resized string\n";
    }

    dump_mem_info(".mem_info_after_100mb.txt");

    heap_info = om::heap_info::get_heap_process_segment_layout();

    print(heap_info);

    return cout.fail();
}
